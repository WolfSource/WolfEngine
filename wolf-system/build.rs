use core::panic;
use git2::{IntoCString, Repository};
use std::{collections::HashMap, path::Path};

const MACOSX_DEPLOYMENT_TARGET: &str = "12.0"; //empty string means get the latest version from system

enum BuildType {
    CMAKE = 0,
    MAKE,
    SHELL,
}

type BuildConfig = (
    BuildType,                 /* build type */
    &'static str,              /* git url */
    &'static str,              /* relative path to the CMake file from root of it's repository*/
    Vec<&'static str>,         /* CMake configures */
    bool,         /* true means linking static library, false means linking dynamic library*/
    &'static str, /* relative path to the cxx files */
    &'static str, /* relative path to the rust file related to cxx */
    &'static str, /* prefix path to the libs & includes (will be used for MakeFile) */
    Vec<(&'static str, bool)>, /* paths to the dependencies libraries. when bool is true, means STATIC Linking  */
);

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let mut git_sources: HashMap<&str, BuildConfig> = HashMap::new();
    git_sources.insert(
        "lz4",
        (
            BuildType::CMAKE,
            "https://github.com/lz4/lz4.git",
            "build/cmake/",
            ["-DBUILD_SHARED_LIBS=FALSE", "-DBUILD_STATIC_LIBS=TRUE"].to_vec(),
            true,
            "src/compression/cxx/lz4/",
            "src/compression/lz4.rs",
            "",
            [].to_vec(),
        ),
    );
    git_sources.insert(
        "lzma",
        (
            BuildType::CMAKE,
            "https://github.com/WolfEngine/lzma.git",
            "",
            [].to_vec(),
            true,
            "src/compression/cxx/lzma/",
            "src/compression/lzma.rs",
            "",
            [].to_vec(),
        ),
    );

    let lua_lib_name: &str;
    let build_type: BuildType;
    let deps: Vec<(&'static str, bool)>;
    if cfg!(windows) {
        lua_lib_name = "luajit";
        deps = [("lua51", true)].to_vec(); //link lua51 as static
        build_type = BuildType::SHELL;
    } else {
        lua_lib_name = "luajit-5.1";
        deps = [].to_vec();
        build_type = BuildType::MAKE;
    };

    git_sources.insert(
        lua_lib_name,
        (
            build_type,
            "https://github.com/LuaJIT/LuaJIT.git",
            "",
            [].to_vec(),
            true,
            "src/script/cxx/luaJIT/",
            "src/script/lua.rs",
            "/usr/local/",
            deps,
        ),
    );

    //first build proto buffers
    let paths = std::fs::read_dir("./proto")?;
    for path in paths {
        let p = path?.path();
        if let Some(extension) = p.extension() {
            if extension == "proto" {
                tonic_build::compile_protos(p)?;
            }
        }
    }

    //get the current path
    let current_dir_path = std::env::current_dir().expect("could not get current directory");
    let current_dir = current_dir_path
        .to_str()
        .expect("could not get a &str from current directory");

    //get current build profile
    let profile_str = std::env::var("PROFILE").expect("Build failed: could not get PROFILE");
    //get opt_level
    let opt_level_str =
        std::env::var("OPT_LEVEL").expect("Build failed: could not get OPT_LEVEL profile");
    //set cmake build config
    let build_profile = match &opt_level_str[..] {
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
    };
    //get current target os
    let target_os =
        std::env::var("CARGO_CFG_TARGET_OS").expect("Build failed: could not get target OS");

    // make sure set the necessery enviroment variables for OSX
    if target_os == "macos" {
        if MACOSX_DEPLOYMENT_TARGET.is_empty() {
            let file = std::fs::read_to_string("/System/Library/CoreServices/SystemVersion.plist")?;
            let cur = std::io::Cursor::new(file.as_bytes());
            let v = plist::Value::from_reader(cur)?;

            let version = v
                .as_dictionary()
                .and_then(|d| d.get("ProductVersion")?.as_string())
                .expect("SystemVersion.plist is not a dictionary");
            std::env::set_var("MACOSX_DEPLOYMENT_TARGET", version);
        } else {
            std::env::set_var("MACOSX_DEPLOYMENT_TARGET", MACOSX_DEPLOYMENT_TARGET);
        }
    }

    // tem vectors for storing rust & cxx sources
    let mut rust_srcs: Vec<String> = Vec::new();
    let mut cxx_srcs: Vec<String> = Vec::new();
    let mut include_srcs: Vec<String> = Vec::new();
    let mut lib_paths: HashMap<String, (String, bool)> = HashMap::new();
    let mut lib_deps = Vec::<(&'static str, bool)>::new();

    // iterate over git repositories
    for (k, mut v) in git_sources {
        //store deps libraries
        lib_deps.append(&mut v.8);

        let prefix_path = v.7;

        //git project just opened
        let build_dir = format!("{}/deps/{}/build/{}", current_dir, k, build_profile);
        println!(
            "cargo:rustc-link-search=native={}{}/lib",
            build_dir, prefix_path
        );

        //check git project already exists
        let git_repo_path = format!("{}/deps/{}", current_dir, k);
        let ret = Repository::open(git_repo_path.clone())
            .and_then(|_r| {
                //if build folder is not exist, we should build it again
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
                let ret = Repository::clone_recurse(v.1, git_repo_path.clone())
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
                    match v.0 {
                        BuildType::CMAKE => {
                            build_cmake(&build_profile, &git_repo_path, &v);
                        }
                        BuildType::MAKE => {
                            build_make(&build_profile, &opt_level_str, &git_repo_path, &v);
                        }
                        BuildType::SHELL => {
                            //build with custom shell script should be implement
                        }
                    }
                }

                //path to the cmake folder
                let path_to_cmake_folder = v.2;
                //link library static or dynamic
                let link_static = v.4;
                //cxx src path
                let cxx_src_path = v.5;
                //store it for later
                cxx_srcs.push(cxx_src_path.to_string());

                //rust src path
                let rust_src_path = v.6;
                //store it for later
                rust_srcs.push(rust_src_path.to_string());

                let prefix_path = v.7;

                //include library includes
                let path_to_include = format!(
                    "{}/{}build/{}/{}/include/",
                    git_repo_path, path_to_cmake_folder, build_profile, prefix_path
                );
                include_srcs.push(path_to_include.to_string());

                //link to the libraries
                let path_to_lib = format!(
                    "{}/{}build/{}/{}/lib/",
                    git_repo_path, path_to_cmake_folder, build_profile, prefix_path
                );
                lib_paths.insert(k.to_string(), (path_to_lib.to_string(), link_static));
            }
            Err(e) => {
                panic!("Build failed: error {:?}", e);
            }
        }
    }

    // build cxx lib
    let mut build_cxx = cxx_build::bridges(rust_srcs);
    //build_cxx.flag_if_supported("/std:c++latest");
    build_cxx.flag_if_supported("-std=c++17");
    build_cxx.flag_if_supported("-std=c17");
    build_cxx.flag_if_supported("-Wall");
    build_cxx.flag_if_supported("-fPIC");

    //add DEBUG or NDEBUG flags
    if profile_str == "debug" {
        build_cxx.define("DEBUG", "DEBUG");
    } else {
        build_cxx.define("NDEBUG", "NDEBUG");
    }

    //add necessery includes/libs or defines based on target OS
    match target_os.as_ref() {
        "macos" => {
            build_cxx.include("/usr/local/include/");
        }
        "linux" => {
            build_cxx.include("/usr/local/include/");
        }
        "windows" => {
            build_cxx.define("WIN32", "WIN32");
            build_cxx.define("_WINDOWS", "_WINDOWS");

            if profile_str == "debug" {
                println!("cargo:rustc-link-lib=msvcrtd");
            } else {
                println!("cargo:rustc-link-lib=msvcrt");
            }
        }
        _ => panic!("Build failed: target os \"{}\" undefined", target_os),
    }

    // include all cxx files
    for iter in cxx_srcs {
        //include all c/cpp files
        let _ = std::fs::read_dir(&iter)
            .and_then(|paths| {
                for iter in paths {
                    match iter {
                        Ok(path) => {
                            let p = path.path();
                            let ext = p
                                .extension()
                                .unwrap_or(std::ffi::OsStr::new(""))
                                .to_ascii_lowercase();
                            if p.is_file() {
                                let file_path_os_string = p.into_os_string();
                                let file_path_str = file_path_os_string.to_str().unwrap();
                                println!("cargo:rerun-if-changed={}", file_path_str);
                                if ext == "c"
                                    || ext == "cc"
                                    || ext == "cpp"
                                    || ext == "cxx"
                                    || ext == "c++"
                                {
                                    build_cxx.file(Path::new(file_path_str));
                                }
                            }
                        }
                        Err(_e) => {}
                    }
                }
                Ok(())
            })
            .or_else(|e| Err(e));
    }

    //include all includes from CMAKEs
    for iter in include_srcs {
        println!("cargo:rerun-if-changed={}", iter);
        build_cxx.include(Path::new(&iter));
    }

    //link dependencies
    for (lib, static_) in lib_deps {
        if static_ {
            println!("cargo:rustc-link-lib=static={}", lib);
        } else {
            println!("cargo:rustc-link-lib=dylib={}", lib);
        }
    }

    //link libraries generated by CMAKEs
    for iter in lib_paths {
        println!("cargo:rerun-if-changed={}", iter.1 .0);
        println!("cargo:rustc-link-search=native={}", iter.1 .0);
        if iter.1 .1 {
            println!("cargo:rustc-link-lib=static={}", iter.0);
        } else {
            println!("cargo:rustc-link-lib=dylib={}", iter.0);
        }
    }

    //rename cxx library to prevent con
    build_cxx.compile("wolf_system_cxx");

    Ok(())
}

fn build_cmake(p_cmake_build_type: &str, p_git_repo_path: &str, p_value: &BuildConfig) {
    use std::process::Command;

    //get path to CMakeLists file
    let cmake_parent_path = format!("{}/{}", p_git_repo_path, p_value.2);
    //get cmake configs
    let cmake_config_args = &p_value.3;

    //check build folder
    let build_folder_str = format!("{}/build", cmake_parent_path);
    let build_folder_path = Path::new(&build_folder_str);
    if !build_folder_path.exists() {
        std::fs::create_dir(build_folder_path).expect(
            format!(
                "could not create build folder for cmake {}CMakeLists.txt",
                cmake_parent_path
            )
            .as_str(),
        );
    }
    //set installfolder
    let install_folder_str = format!("{}/{}", build_folder_str, p_cmake_build_type);

    //configure cmake
    let mut out = Command::new("cmake")
        .current_dir(&cmake_parent_path)
        .args(["-Wdev", "--debug-output", "-B", "build"])
        .arg(".")
        .args(cmake_config_args)
        .arg(format!("-DCMAKE_INSTALL_PREFIX:PATH={}", install_folder_str).as_str())
        .arg(format!("-DCMAKE_BUILD_TYPE={}", p_cmake_build_type).as_str())
        .output()
        .expect(
            format!(
                "Build failed: could not configure cmake for {}CMakeLists.txt",
                cmake_parent_path
            )
            .as_str(),
        );
    if !out.status.success() {
        panic!(
            "Build failed: CMake project was not configured because: {:?}",
            out.stderr.into_c_string()
        );
    }

    // build cmake
    out = Command::new("cmake")
        .current_dir(&cmake_parent_path)
        .args([
            "--build",
            "build",
            "--target",
            "install",
            "--config",
            p_cmake_build_type,
            "--parallel",
            "8",
        ])
        .output()
        .expect(
            format!(
                "Build failed: could not run cmake for {}CMakeLists.txt",
                cmake_parent_path
            )
            .as_str(),
        );
    if !out.status.success() {
        panic!(
            "CMake Build failed for {} because: {:?}",
            cmake_parent_path,
            out.stderr.into_c_string()
        );
    }
}

fn build_make(
    p_make_build_profile: &str,
    _p_make_opt_level: &str,
    p_git_repo_path: &str,
    p_value: &BuildConfig,
) {
    //get path to make file
    let make_parent_path = format!("{}/{}", p_git_repo_path, p_value.2);
    //check build folder
    let build_folder_str = format!("{}/build", make_parent_path);
    //check install folder
    let install_folder_str = format!("{}/{}", build_folder_str, p_make_build_profile);

    make_folder(&make_parent_path);
    make_folder(&build_folder_str);
    make_folder(&install_folder_str);

    // let c_flag: String;
    // let cxx_flag: String;
    // if p_make_build_profile == "Debug" || p_make_build_profile == "RelWithDebInfo" {
    //     c_flag = format!("CFLAGS='-g -O{} -w'", p_make_opt_level);
    //     cxx_flag = format!("CXXFLAGS='-g -O{} -w'", p_make_opt_level);
    // } else {
    //     c_flag = format!("CFLAGS='-O{} -w'", p_make_opt_level);
    //     cxx_flag = format!("CXXFLAGS='-O{} -w'", p_make_opt_level);
    // }

    let install_flag = format!("DESTDIR={}", &install_folder_str);

    //configure cmake
    let out = std::process::Command::new("make")
        .current_dir(&make_parent_path)
        .args(["install", &install_flag])
        .output()
        .expect(
            format!(
                "Build failed: could not build make for {}MakeFile",
                make_parent_path
            )
            .as_str(),
        );

    if !out.status.success() {
        panic!(
            "Build failed: Make project was not build because: {:?}",
            out.stderr.into_c_string()
        );
    }
}

fn make_folder(path: &str) {
    //check folder exists
    let p = Path::new(&path);
    if !p.exists() {
        std::fs::create_dir(p).expect(format!("could not create folder {}", path).as_str());
    }
}
