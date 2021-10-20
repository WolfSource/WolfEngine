use cmake;
use core::panic;
use git2::Repository;
use std::{collections::HashMap, path::Path};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    //c/c++ dependencies repos
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

    // iterate over git repositories
    for (k, v) in git_sources.into_iter() {
        //check git project already exists
        let path = format!("{}/deps/{}", current_dir, k);
        let ret = Repository::open(path.clone())
            .and_then(|_r| {
                //git project just opened
                Ok(())
            })
            .or_else(|_e| {
                // try clone it again
                let ret = Repository::clone(v.0, path.clone())
                    .and_then(|_repo| {
                        //Ok, now try build it
                        let build_type = v.1 .0;
                        match build_type {
                            //CBuildType::CXX => {}
                            CBuildType::CMAKE => {
                                let cmake_path = v.1 .1;
                                let flags = &v.1 .2;

                                //build cmake
                                let mut build = cmake::Config::new(cmake_path);
                                for &flag in flags {
                                    build.configure_arg(flag);
                                }
                                let build_dir = build.build();
                                let build_dir_str = build_dir.as_path().to_str().expect(
                                    format!(
                                        "Build failed: could not get build directory of {:?}",
                                        build_dir
                                    )
                                    .as_ref()
                                );

                                //closure for using header to bindgen
                                let binding_closure = |p_header_name: &str, p_dst_path: &Path| -> () {
                                    let b = bindgen::Builder::default()
                                        // The input header we would like to generate
                                        // bindings for.
                                        .header(p_header_name)
                                        // Tell cargo to invalidate the built crate whenever any of the
                                        // included header files changed.
                                        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
                                        // Finish the builder and generate the bindings.
                                        .generate()
                                        // Unwrap the Result and panic on failure.
                                        .expect(
                                            format!(
                                            "Build failed: unable to generate bindings for {:?}",
                                            p_header_name
                                        )
                                            .as_ref()
                                        );

                                    if std::fs::metadata(p_dst_path).is_err()
                                    {
                                        //create dir
                                        std::fs::create_dir_all(p_dst_path).expect(
                                            format!(
                                            "Build failed: could not create directory for {:?} in order to copy generated rust file {}",
                                            p_dst_path,
                                            p_header_name
                                        )
                                            .as_ref());
                                    }

                                    let name = Path::new(&p_header_name).file_stem().expect( format!(
                                        "Build failed: could not get name from header {}",
                                        p_header_name
                                    )
                                    .as_ref()).to_str().unwrap();

                                    let dst_path = p_dst_path.join(format!("{}.rs", name));
                                    b.write_to_file(dst_path).expect(
                                        format!(
                                            "Build failed: could not write generated rust file of {} to {:?}",
                                            p_header_name, p_dst_path
                                        )
                                        .as_ref()
                                    );
                                };

                                //convert headers to rust codes
                                let include = format!("{}/include", build_dir_str);
                                for content in fs_extra::dir::get_dir_content(include.clone()) {
                                    for header in content.files {
                                        let to_str = format!("{}/{}/c_{}/", current_dir, v.1 .5, k);
                                        let to = Path::new(&to_str);
                                        binding_closure(&header, to);
                                    }
                                }

                                //get build dir and copy it to the root of repo

                                let mut from_paths = Vec::new();
                                from_paths.push(build_dir_str);

                                let dst = format!("{}", path);
                                let options = fs_extra::dir::CopyOptions::new();

                                fs_extra::copy_items(&from_paths, dst, &options).expect(
                                    format!(
                                        "Build failed: could not copy build directory {:?}",
                                        build_dir
                                    )
                                    .as_ref()
                                );
                                //remove source files
                                let _ = fs_extra::remove_items(&from_paths);
                            }
                        }
                        Ok(())
                    })
                    .or_else(|e| Err(e));
                ret
            });

        if ret.is_ok() {
            // git project exists, so tell Cargo that rerun this build script when the given folder changes.
            println!("cargo:rerun-if-changed={}", path);

            //link lib
            let link_static = v.1 .3;
            let lib = v.1 .4;
            println!("cargo:rustc-link-search=native={}/out/lib/", path);
            if link_static {
                println!("cargo:rustc-link-lib=static={}", lib);
            } else {
                println!("cargo:rustc-link-lib=dylib={}", lib);
            }
        } else {
            panic!("Build failed: error {:?}", ret);
        }
    }

    Ok(())
}
