use std::{
    ffi::OsStr,
    path::{Path, PathBuf},
    process::Command,
};

#[derive(Clone, Debug)]
pub struct Define<'a> {
    pub key: &'a str,
    pub value: Option<&'a str>,
}

#[derive(Clone, Debug)]
pub enum FetchContentType {
    NOP = 0,
    Vcpkg,
}

#[derive(Clone, Debug)]
pub enum BuildType<'a> {
    NOP,
    CMake(
        PathBuf,       /* path to the CMake file */
        &'a [&'a str], /* cmake config args */
    ),
}

impl BuildType<'_> {
    /// # Panics
    ///
    /// Panics if `output` is not formatted correctly or if one of the underlying
    /// compiler commands fails. It can also panic if it fails reading file names
    /// or creating directories.
    pub fn build(&self) {
        match self {
            BuildType::NOP => { /* No operation */ }
            BuildType::CMake(p_cmake_root_dir, p_cmake_config_args) => {
                run_cmake(p_cmake_root_dir, p_cmake_config_args);
            }
        }
    }
}

#[derive(Clone, Debug)]
pub struct Dependency<'a> {
    // name of dependency
    pub name: &'a str,
    // fetch type
    pub fetch_content: FetchContentType,
    // if headers set to the None, wolf will be iterate over all headers within includes paths of vcpkg
    pub headers: Vec<PathBuf>,
    // an output directory which will contain generated rust file
    pub rust_output_dir: &'a PathBuf,
}

#[derive(Clone, Debug)]
pub struct Session<'a> {
    // name of build session
    pub name: &'a str,
    // dependencies of build
    pub deps: &'a [Dependency<'a>],
    // the build type
    pub build_type: BuildType<'a>,
}

impl Session<'_> {
    /// # Panics
    ///
    /// Will panic if profile, target os or vcpkg environment variable could not fetch
    pub fn run(&mut self) {
        // build
        self.build_type.build();

        // generate rust sources from c headers
        let mut includes = Vec::<PathBuf>::new();
        for dep in self.deps {
            // create path buffer
            let mut include_prefix = PathBuf::new();

            // fetch deps
            match dep.fetch_content {
                FetchContentType::NOP => {
                    // append all headers
                    for iter in &dep.headers {
                        includes.push(PathBuf::from(iter));
                    }
                }
                FetchContentType::Vcpkg => {
                    // find package from vcpkg
                    let vcpkg = vcpkg::Config::new()
                        .emit_includes(true)
                        .find_package(dep.name)
                        .unwrap();
                    // link includes and libs
                    for meta_data in vcpkg.cargo_metadata {
                        println!("{}", meta_data);
                    }
                    // create bindgens from provided headers
                    if !vcpkg.include_paths.is_empty() {
                        if dep.headers.is_empty() {
                            println!("iter");
                            //TODO iterate over all sub directories of vcpkg includes and fetch .h & .hpp
                            //for iter in sub dir of  {
                            //  includes.push(PathBuf::from(iter));
                            //}
                        } else {
                            include_prefix = vcpkg.include_paths[0].clone();
                        }
                    }
                }
            }

            // create rust sources via bindgen
            for header in &dep.headers {
                let include_path_buf = include_prefix.join(header);
                // create header path
                let (name, path) = get_header_info(&include_path_buf);
                // convert header to rust file
                self.execute_bindgen(name, path, dep.rust_output_dir);
                // add to includes
                includes.push(include_path_buf);
            }
        }
    }

    fn execute_bindgen(&self, p_header_name: &str, p_header_path: &str, p_output_path: &Path) {
        // convert header to rust code via binding
        let bindings = bindgen::Builder::default()
            // The input header we would like to generate
            // bindings for.
            .header(p_header_path)
            // Finish the builder and generate the bindings.
            .generate()
            // Unwrap the Result and panic on failure.
            .unwrap_or_else(|_| {
                panic!(
                    "unable to generate bindings for {}/{}",
                    self.name, p_header_path
                )
            });

        // write to the rust file
        let out_path = p_output_path.join(format!("{}.rs", p_header_name));
        bindings
            .write_to_file(out_path.clone())
            .unwrap_or_else(|e| {
                panic!(
                    "couldn't write bindings for {} header {:?} because of {}",
                    self.name, out_path, e
                )
            });
    }
}

fn get_header_info(p_path_buffer: &Path) -> (&str, &str) {
    // get header name as Os String
    let header_name_os_str = p_path_buffer.file_stem().unwrap_or_else(|| {
        panic!("could't get header name as OsStr from {:?}", p_path_buffer);
    });
    // get header name from OsStr
    let header_name = header_name_os_str.to_str().unwrap_or_else(|| {
        panic!("could't get header name{:?}", header_name_os_str);
    });
    let header_path = p_path_buffer.to_str().unwrap_or_else(|| {
        panic!(
            "could't convert header path buffer to str {:?}",
            p_path_buffer
        );
    });
    (header_name, header_path)
}

pub fn query_by_ext_then_exec_func_foreach<F>(
    p_ext_types: &[&str],
    p_root_dirs: &[&str],
    p_func: &mut F,
) where
    F: FnMut(&Path /* file path */, &OsStr /* file extension */),
{
    // iterate over all provided directories
    for iter in p_root_dirs {
        let dir = *iter;
        //include all c/cpp files
        let _r = std::fs::read_dir(dir).map(|paths| {
            for iter in paths {
                match iter {
                    Ok(path) => {
                        let p = path.path();
                        if p.is_file() {
                            let ext = p.extension().unwrap_or_default().to_ascii_lowercase();
                            let mut exe_is_supported = false;
                            for iter in p_ext_types {
                                if ext == *iter {
                                    exe_is_supported = true;
                                    break;
                                }
                            }
                            if exe_is_supported {
                                p_func(&p, &ext);
                            }
                        }
                    }
                    Err(_e) => {}
                }
            }
        });
    }
}

#[must_use]
pub fn get_build_profile<'a>() -> &'a str {
    // //get current build profile
    let profile_str = std::env::var("PROFILE").expect("Couldn't get PROFILE");
    //get opt_level
    let opt_level_str = std::env::var("OPT_LEVEL").expect("Couldn't get OPT_LEVEL profile");
    //set CMake build config
    match &opt_level_str[..] {
        "0" => "Debug",
        "1" | "2" | "3" => {
            if profile_str == "debug" {
                "RelWithDebInfo"
            } else {
                "Release"
            }
        }
        "s" | "z" => "MinSizeRel",
        _ => {
            if profile_str == "debug" {
                "Debug"
            } else {
                "Release"
            }
        }
    }
}

fn run_cmake<'a>(p_cmake_root_dir: &Path, p_cmake_config_args: &'a [&'a str]) {
    // get build profile
    let build_profile = get_build_profile();
    // check build folder
    let build_folder = p_cmake_root_dir.join("build");
    if !build_folder.exists() {
        std::fs::create_dir(build_folder.clone()).unwrap_or_else(|_| {
            panic!(
                "couldn't create build folder for CMake {:?}",
                p_cmake_root_dir
            )
        });
    }
    let install_folder = build_folder.join(build_profile);
    if !install_folder.exists() {
        std::fs::create_dir(install_folder.clone()).unwrap_or_else(|_| {
            panic!(
                "couldn't create install folder for CMake {:?}",
                install_folder
            )
        });
    }

    // configure cmake
    let mut out = Command::new("cmake")
        .current_dir(&p_cmake_root_dir)
        .args(["-Wdev", "--debug-output", "-B", "build"])
        .arg(".")
        .args(p_cmake_config_args)
        .arg(&format!(
            "-DCMAKE_INSTALL_PREFIX:PATH={}",
            install_folder.to_str().unwrap()
        ))
        .arg(&format!("-DCMAKE_BUILD_TYPE={}", build_profile))
        .output()
        .unwrap_or_else(|_| panic!("couldn't configure cmake for {:?}", install_folder));

    assert!(
        out.status.success(),
        "CMake project was not configured. stdout:{:?} stderr:{:?}",
        std::str::from_utf8(&out.stdout),
        std::str::from_utf8(&out.stderr)
    );

    // build cmake
    out = Command::new("cmake")
        .current_dir(&p_cmake_root_dir)
        .args([
            "--build",
            "build",
            "--target",
            "install",
            "--config",
            build_profile,
            "--parallel",
            "8",
        ])
        .output()
        .unwrap_or_else(|_| panic!("couldn't run cmake for {:?}", install_folder));

    assert!(
        out.status.success(),
        "CMake build failed for {:?} stdout:{:?} stderr:{:?}",
        install_folder,
        std::str::from_utf8(&out.stdout),
        std::str::from_utf8(&out.stderr)
    );
}

// let mut rust_srcs = Vec::new();
// let mut cpp_srcs = Vec::new();
// let mut includes: &[&str] = &["."];
// if let Some(incs) = self.includes {
//     includes = incs;
// }

//     if let Some(deps) = self.deps {
//

//             // add to cpp & rust sources
//             rust_srcs.append(&mut dep.rust_src_paths.to_vec());
//             get_all_cpp_sources(dep.cpp_src_paths, &mut cpp_srcs);
//         }
//     }

//     let cpp_iso_version: &str;
//     let win32_val: &str;
//     #[cfg(target_os = "windows")]
//     {
//         // set c++ iso standard version
//         cpp_iso_version = "/std:c++latest";
//         // define WIN32
//         win32_val = "1";
//         // link MSVC
//         if profile_str == "debug" {
//             println!("cargo:rustc-link-lib=msvcrtd");
//         } else {
//             println!("cargo:rustc-link-lib=msvcrt");
//         }
//     }
//     #[cfg(not(target_os = "windows"))]
//     {
//         cpp_iso_version = "-std=c++20";
//         win32_val = "0";
//     }
//     //add DEBUG or NDEBUG flags
//     let define_dubug = if profile_str == "debug" {
//         "DEBUG"
//     } else {
//         "NDEBUG"
//     };

//     // build cxx lib
//     cxx_build::bridges(rust_srcs)
//         .files(cpp_srcs)
//         .includes(includes)
//         .flag_if_supported(cpp_iso_version)
//         .flag_if_supported("-Wall")
//         .flag_if_supported("-fPIC")
//         .flag_if_supported("-Wc++11-inline-namespace")
//         .flag_if_supported("-EHsc")
//         .define("WIN32", win32_val)
//         .define(define_dubug, "1")
//         .compile(self.name);
