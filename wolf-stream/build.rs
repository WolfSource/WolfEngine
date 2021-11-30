use core::panic;
use git2::Repository;
use std::{collections::HashMap, path::Path};

const MACOSX_DEPLOYMENT_TARGET: &str = "12.0"; //empty string means get the latest version from system

enum BuildType {
    // CMAKE = 0,
    // MAKE,
    SHELL,
}

type BuildConfig = (
    BuildType,           /* build type */
    &'static str,        /* git url */
    &'static str,        /* relative path to the CMake file from root of it's repository*/
    Vec<&'static str>,   /* CMake configures */
    bool,                /* true means linking static library, false means linking dynamic library*/
    &'static str,        /* relative path to the cxx files */
    &'static str,        /* relative path to the rust file related to cxx */
    &'static str,        /* prefix path to the libs & includes (will be used for MakeFile) */
    Vec<(String, bool)>, /* paths to the dependencies libraries. when bool is true, means STATIC Linking  */
    Vec<String>,         /* commands to run (will be used for shell) */
);

fn main() -> core::result::Result<(), Box<dyn std::error::Error>> {
    env!(
        "PKG_CONFIG_PATH",
        "$PKG_CONFIG_PATH enviroment variable was not set"
    );
    let mut configure_flags: String = " ".to_string();
    let target_os_ffmpeg: &str;
    let target_os_live555: &str;
    let target_os =
        std::env::var("CARGO_CFG_TARGET_OS").expect("Build failed: could not get target OS");

    if target_os == "windows" {
        target_os_ffmpeg = "win";
        target_os_live555 = "./genWindowsMakefiles";
    } else if target_os == "macos" {
        target_os_ffmpeg = "darwin";
        target_os_live555 = "./genMakefiles macosx-bigsur";
    } else if target_os == "linux" {
        target_os_ffmpeg = "linux";
        target_os_live555 = "./genMakefiles linux-64bit";
    } else {
        panic!("build for target_os not defined.");
    }

    let target_arch = std::env::consts::ARCH;
    //get current build profile
    let profile_str = std::env::var("PROFILE").expect("Build failed: could not get PROFILE");

    if profile_str == "debug" {
        configure_flags = "--enable-debug".to_string();

        if target_os == "windows" {
            configure_flags = format!(
                "{} --extra-cflags=-MDd --extra-ldflags=/NODEFAULTLIB:libcmt",
                configure_flags
            );
        };
    }

    //get the current path
    let current_dir_path = std::env::current_dir().expect("could not get current directory");
    let current_dir = current_dir_path
        .to_str()
        .expect("could not get a &str from current directory");

    //get opt_level
    let opt_level_str =
        std::env::var("OPT_LEVEL").expect("Build failed: could not get OPT_LEVEL profile");
    //set build config
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

    let mut git_sources: HashMap<&str, BuildConfig> = HashMap::new();
    git_sources.insert(
        "ffmpeg",
        (
            BuildType::SHELL,
            "https://github.com/FFmpeg/FFmpeg.git",
            "",
            [].to_vec(),
            false,
            "src/rtsp/client/cxx/",
            "src/rtsp/client/rtsp_client.rs",
            "",
            [
                ("avcodec".to_string(), false),
                ("avdevice".to_string(), false),
                ("avfilter".to_string(), false),
                ("avformat".to_string(), false),
                ("avutil".to_string(), false),
                ("swresample".to_string(), false),
                ("swscale".to_string(), false),
            ]
            .to_vec(),
            [
                "#!/bin/bash\r\n".to_string(),
                "cd $1 && ".to_string(),
                "./configure".to_string(),
                "--enable-asm".to_string(),
                "--enable-yasm".to_string(),
                "--disable-doc".to_string(),
                "--disable-ffplay".to_string(),
                "--disable-ffprobe".to_string(),
                "--disable-ffmpeg".to_string(),
                "--enable-shared".to_string(),
                "--disable-static".to_string(),
                "--disable-bzlib".to_string(),
                "--disable-libopenjpeg".to_string(),
                "--disable-iconv".to_string(),
                "--disable-zlib".to_string(),
                format!(
                    "--prefix={}/deps/{}/build/{}",
                    current_dir, "ffmpeg", build_profile
                ),
                format!("--target-os={} ", target_os_ffmpeg),
                format!("--arch={} ", target_arch),
                format!("{} ", configure_flags),
                "&& make clean".to_string(),
                "&& make".to_string(),
                "&& make install ".to_string(),
            ]
            .to_vec(),
        ),
    );

    git_sources.insert(
        "live555",
        (
            BuildType::SHELL,
            "",
            "",
            [].to_vec(),
            false,
            "src/rtsp/server/cxx/",
            "src/rtsp/server/rtsp_server_live555.rs",
            "/usr/local/",
            [
                ("BasicUsageEnvironment".to_string(), true),
                ("groupsock".to_string(), true),
                ("liveMedia".to_string(), true),
                ("UsageEnvironment".to_string(), true)
            ].to_vec(),
            [
                "#!/bin/bash\r\n".to_string(),
                "cd $1 && ".to_string(),
                "curl http://www.live555.com/liveMedia/public/live555-latest.tar.gz > ./live555-latest.tar.gz && ".to_string(),
                "tar -xzvf live555-latest.tar.gz --strip-components 1 && ".to_string(),
                format!("{} && ", target_os_live555),
                "make clean && ".to_string(),
                "make && ".to_string(),
                format!("make install DESTDIR={}/deps/{}/build/{}",  current_dir, "live555", build_profile),
            ]
            .to_vec(),
        ),
    );

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
    let mut lib_deps = Vec::<(String, bool)>::new();

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
                let url = v.1;
                if !url.is_empty() {
                    let ret = Repository::clone_recurse(url, git_repo_path.clone())
                        .and_then(|_repo| {
                            //Time to build it
                            Ok(true)
                        })
                        .or_else(|e| Err(e));
                    ret
                } else {
                    //those projects which do not have git repos
                    if !Path::new(&build_dir).exists() {
                        //rebuild it again
                        Ok(true)
                    } else {
                        //no need to build it again
                        Ok(false)
                    }
                }
            });

        match ret {
            Ok(build) => {
                if build {
                    match v.0 {
                        BuildType::SHELL => {
                            build_shell(&build_profile, &opt_level_str, &git_repo_path, &v, &v.9);
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

                //include library includes
                let path_to_include = format!(
                    "{}/{}build/{}{}/include/",
                    git_repo_path, path_to_cmake_folder, build_profile, prefix_path
                );
                include_srcs.push(path_to_include.to_string());

                //link to the libraries
                let path_to_lib = format!(
                    "{}/{}build/{}{}/lib/",
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
    println!("cargo:rerun-if-changed=/Users/pooyaeimandar/Documents/github/WolfEngine/wolf/wolf-stream/deps/live555/build/Debug/usr/local/lib/");
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
    build_cxx.compile("wolf_stream_cxx");

    Ok(())
}

fn build_shell(
    p_make_build_profile: &str,
    _p_make_opt_level: &str,
    p_git_repo_path: &str,
    p_value: &BuildConfig,
    p_command: &Vec<String>,
) {
    //get path to make file
    let shell_script_parent_path = format!("{}/{}", p_git_repo_path, p_value.2);
    //check build folder
    let build_folder_str = format!("{}build", shell_script_parent_path);
    //check install folder
    let install_folder_str = format!("{}/{}", build_folder_str, p_make_build_profile);

    make_folder(&shell_script_parent_path);
    make_folder(&build_folder_str);
    make_folder(&install_folder_str);

    let shell_command = format!("{}", &p_command.join(" "));
    let options = run_script::types::ScriptOptions::new();
    let (output, error) =
        run_script::run_script_or_exit!(shell_command, &vec![shell_script_parent_path], options);
    if !error.is_empty() {
        panic!(
            "shell script returns with error. output:{} error:{}",
            output, error
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
