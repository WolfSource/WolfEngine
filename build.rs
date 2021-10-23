use cmake;
use core::panic;
use git2::Repository;
use std::{
    collections::HashMap,
    path::{Path, PathBuf},
};

//c/c++ build types
#[derive(Clone)]
enum CBuildType {
    CMAKE,
    //CXX,
}
type BuildConfig = (
    CBuildType,        /* the specific c library build type */
    &'static str,      /* relative path to the cmake file */
    Vec<&'static str>, /* cmake configures */
    bool,              /* true means linking static false means linking dynamic */
    &'static str,      /* lib path */
    &'static str,      /* bindgen dst path */
);

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let mut git_sources: HashMap<&str, (&str, BuildConfig)> = HashMap::new();
    // git_sources.insert(
    //     "base64",
    //     (
    //         "https://github.com/aklomp/base64.git",
    //         (CBuildType::CC, "", ""),
    //     ),
    // );
    git_sources.insert(
        "lz4",
        (
            "https://github.com/lz4/lz4.git",
            (
                CBuildType::CMAKE,
                "deps/lz4/build/cmake/",
                ["-DBUILD_SHARED_LIBS=FALSE", "-DBUILD_STATIC_LIBS=TRUE"].to_vec(),
                true,
                "lz4",
                "src/compression/",
            ),
        ),
    );

    //get the current path
    let current_dir_path = std::env::current_dir().expect("could not get current directory");
    let current_dir = current_dir_path
        .to_str()
        .expect("could not get a &str from current directory");

    //get current build profile
    let profile = std::env::var("PROFILE").expect("could not get build profile");

    // iterate over git repositories
    for (k, v) in git_sources.into_iter() {
        //check git project already exists
        let git_repo_path = format!("{}/deps/{}", current_dir, k);
        let ret = Repository::open(git_repo_path.clone())
            .and_then(|_r| {
                //git project just opened
                let build_dir = format!("{}/deps/{}/build/{}", current_dir, k, profile);
                if !Path::new(&build_dir).exists() {
                    //rebuild it again
                    Ok(true)
                } else {
                    //no need to build it again
                    Ok(false)
                }
            })
            .or_else(|_e| {
                // try clone it again
                let ret = Repository::clone(v.0, git_repo_path.clone())
                    .and_then(|_repo| {
                        //Time to build it
                        Ok(true)
                    })
                    .or_else(|e| Err(e));
                ret
            });

        match ret {
            Ok(build) => {
                if build {
                    let build_type = v.1 .0.clone();
                    match build_type {
                        CBuildType::CMAKE => {
                            build_cmake(&profile, current_dir, &git_repo_path, &k, &v);
                        }
                    }
                }

                //link lib
                let link_static = v.1 .3;
                let lib = v.1 .4;
                println!(
                    "cargo:rustc-link-search=native={}/build/{}/out/lib/",
                    git_repo_path, profile
                );
                if link_static {
                    println!("cargo:rustc-link-lib=static={}", lib);
                } else {
                    println!("cargo:rustc-link-lib=dylib={}", lib);
                }
            }
            Err(e) => {
                panic!("Build failed: error {:?}", e);
            }
        }
    }

    Ok(())
}

fn build_cmake(
    p_build_profile: &str,
    p_current_path: &str,
    p_git_repo_path: &str,
    p_key: &str,
    p_value: &(&str, BuildConfig),
) {
    //get path to CMakeLists file
    let cmake_path = p_value.1 .1;
    //get cmake configs
    let cmake_config_args = &p_value.1 .2;

    //build cmake
    let mut build = cmake::Config::new(cmake_path);
    for &c_arg in cmake_config_args {
        build.configure_arg(c_arg);
    }

    //get the out folder which contains bin/lib/include
    let build_dir = build.build();
    let build_dir_str = build_dir.as_path().to_str().expect(
        format!(
            "Build failed: could not get output build directory of {:?}",
            build_dir
        )
        .as_ref(),
    );

    //closure for generating rust file from c headers via bindgen
    let binding_closure = |p_header_path: &PathBuf, p_dst_path: &Path| -> () {
        let err_msg = format!(
            "Build failed: could not convert header path to &str {:?}",
            p_header_path
        );

        let header_path = p_header_path.to_str().expect(&err_msg);
        let header_name_os_str = p_header_path.file_stem().expect(&err_msg);
        let header_name = header_name_os_str.to_str().expect(&err_msg);

        let b = bindgen::Builder::default()
            // The input header we would like to generate
            .header(header_path)
            // Tell cargo to invalidate the built crate whenever any of the included header files changed.
            .parse_callbacks(Box::new(bindgen::CargoCallbacks))
            // Finish the builder and generate the bindings.
            .generate()
            // Unwrap the Result and panic on failure.
            .expect(
                format!(
                    "Build failed: unable to generate rust bindings from {:?}",
                    header_path
                )
                .as_ref(),
            );

        // check destination directories are exist, if not then create them
        if std::fs::metadata(p_dst_path).is_err() {
            //create output directories for rust files generated by bindgen
            std::fs::create_dir_all(p_dst_path).expect(
                format!(
                "Build failed: could not create directory for {:?} in order to copy generated rust file {}",
                p_dst_path,
                header_path
            )
                .as_ref());
        }

        //use same header name for rust file and save rust file in our project
        let dst_path = p_dst_path.join(format!("{}.rs", header_name));
        b.write_to_file(dst_path).expect(
            format!(
                "Build failed: could not write generated rust file of {} to {:?}",
                header_path, p_dst_path
            )
            .as_ref(),
        );
    };

    //recursive closure for generating rust file from c headers via bindgen
    struct WalkDir<'a> {
        pub f: &'a dyn Fn(&WalkDir, &Path, &str) -> (),
    }
    let walk_dir = WalkDir {
        f: &|p_walk_dir, p_path, p_parent_name| -> () {
            let mut mod_content = String::new();
            let dst_folder = format!("{}/{}/c_{}/", p_current_path, p_value.1 .5, p_parent_name);
            let dst_folder_path = Path::new(&dst_folder);

            //read all files which are located inside this folder
            let ret = std::fs::read_dir(p_path)
                .and_then(|paths| {
                    for path in paths {
                        if path.is_ok() {
                            let p = path.unwrap().path();
                            let p_name = p.file_stem().unwrap().to_str().unwrap();
                            if p.is_file() {
                                mod_content += &format!("pub mod {};\r\n", p_name);

                                binding_closure(&p, dst_folder_path);
                            } else if p.is_dir() {
                                mod_content += &format!("pub mod {};\r\n", p_name);

                                let err_msg = format!("could not get name of {:?}", p);
                                let parent_dir_name_os_str = p.file_stem().expect(&err_msg);
                                let parent_dir_name =
                                    parent_dir_name_os_str.to_str().expect(&err_msg);
                                //call walk directory recursively
                                (p_walk_dir.f)(p_walk_dir, &p, parent_dir_name);
                            }
                        }
                    }
                    Ok(())
                })
                .or_else(|e| Err(e));

            if ret.is_ok() && !mod_content.is_empty() {
                //save pub mod
                let mod_file_path = dst_folder_path.join("mod.rs");
                let err_msg = format!(
                    "could not save mod.rs {} for {:?}",
                    mod_content, mod_file_path
                );
                std::fs::write(mod_file_path.as_path(), mod_content).expect(&err_msg);
            }
        },
    };
    //recursively convert headers to rust codes via bindgen
    let include = format!("{}/include", build_dir_str);
    let include_path = Path::new(&include);
    (walk_dir.f)(&walk_dir, include_path, p_key);

    //move output build directory (out) to the root of repo
    let mut from_paths = Vec::new();
    from_paths.push(build_dir_str);

    let dst = format!("{}/build/{}/", p_git_repo_path, p_build_profile);
    let options = fs_extra::dir::CopyOptions::new();

    if !Path::new(&dst).exists() {
        std::fs::create_dir_all(dst.clone()).expect(
            format!(
                "Build failed: could not create destination build directory {:?}",
                dst
            )
            .as_ref(),
        );
    }

    fs_extra::copy_items(&from_paths, dst.clone(), &options).expect(
        format!(
            "Build failed: could not copy from {:?} to {}",
            build_dir, dst
        )
        .as_ref(),
    );

    //re run build if the following folders changed
    println!("cargo:rerun-if-changed={}/build/", p_git_repo_path);
    println!("cargo:rerun-if-changed={}", dst);

    //remove source files & directory from cache
    let _ = fs_extra::remove_items(&from_paths);
}
