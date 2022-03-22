#[derive(Clone, Debug)]
pub enum BuildType {
    CMake = 0,
    Vcpkg,
}

#[derive(Clone, Debug)]
pub struct DepConfig<'a> {
    pub name: &'a str,
    pub rust_src_paths: &'a [&'a str],
    pub cpp_src_paths: &'a [&'a str],
    pub build_type: BuildType,
}

#[derive(Clone, Debug)]
pub struct BuildSession<'a> {
    pub name: &'a str,
    pub includes: Option<&'a [&'a str]>,
    pub deps: Option<&'a [DepConfig<'a>]>,
}

impl BuildSession<'_> {
    /// # Panics
    ///
    /// Will panic if vcpkg could not find package
    pub fn run(self) {
        // get profile & target of build
        let profile_str = std::env::var("PROFILE").expect("could not get build PROFILE");
        //let target_os = std::env::var("CARGO_CFG_TARGET_OS").expect("could not get target OS");

        let mut rust_srcs = Vec::new();
        let mut cpp_srcs = Vec::new();
        let mut includes: &[&str] = &["."];
        if let Some(incs) = self.includes {
            includes = incs;
        }

        if let Some(deps) = self.deps {
            for dep in deps {
                // find package from vcpkg
                let vcpkg_config = vcpkg::Config::new()
                    .emit_includes(true)
                    .find_package(dep.name)
                    .unwrap();

                // link includes and libs
                for meta_data in vcpkg_config.cargo_metadata {
                    println!("{}", meta_data);
                }

                // add to cpp & rust sources
                rust_srcs.append(&mut dep.rust_src_paths.to_vec());
                get_all_cpp_sources(dep.cpp_src_paths, &mut cpp_srcs);
            }
        }

        let cpp_iso_version: &str;
        let win32_val: &str;
        #[cfg(target_os = "windows")]
        {
            // set c++ iso standard version
            cpp_iso_version = "/std:c++latest";
            // define WIN32
            win32_val = "1";
            // link MSVC
            if profile_str == "debug" {
                println!("cargo:rustc-link-lib=msvcrtd");
            } else {
                println!("cargo:rustc-link-lib=msvcrt");
            }
        }
        #[cfg(not(target_os = "windows"))]
        {
            cpp_iso_version = "-std=c++20";
            win32_val = "0";
        }
        //add DEBUG or NDEBUG flags
        let define_dubug = if profile_str == "debug" {
            "DEBUG"
        } else {
            "NDEBUG"
        };

        // build cxx lib
        cxx_build::bridges(rust_srcs)
            .files(cpp_srcs)
            .includes(includes)
            .flag_if_supported(cpp_iso_version)
            .flag_if_supported("-Wall")
            .flag_if_supported("-fPIC")
            .flag_if_supported("-Wc++11-inline-namespace")
            .flag_if_supported("-EHsc")
            .define("WIN32", win32_val)
            .define(define_dubug, "1")
            .compile(self.name);
    }
}

fn get_all_cpp_sources(p_in: &[&str], p_out: &mut Vec<String>) {
    // include all cpp files
    for iter in p_in {
        let dir = *iter;
        //include all c/cpp files
        let _r = std::fs::read_dir(dir).map(|paths| {
            for iter in paths {
                match iter {
                    Ok(path) => {
                        let p = path.path();
                        let ext = p
                            .extension()
                            .unwrap_or_else(|| std::ffi::OsStr::new(""))
                            .to_ascii_lowercase();
                        if p.is_file() {
                            let file_path_os_string = p.into_os_string();
                            let file_path_str = file_path_os_string.to_str().unwrap();
                            println!("cargo:rerun-if-changed={}", file_path_str);
                            if ext == "c"
                                || ext == "cc"
                                || ext == "cpp"
                                || ext == "cppm"
                                || ext == "ixx"
                                || ext == "cxx"
                                || ext == "c++"
                            {
                                p_out.push(S!(file_path_str));
                            }
                        }
                    }
                    Err(_e) => {}
                }
            }
        });
    }
}
