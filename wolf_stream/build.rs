#![allow(clippy::too_many_lines)]

use core::panic;
use git2::{IntoCString, Repository};
use std::fs::File;
use std::io::prelude::*;
use std::io::{Error, ErrorKind, Read, Result};
use std::{collections::HashMap, fmt::format, path::Path};

const MACOSX_DEPLOYMENT_TARGET: &str = "12.0"; //empty string means get the latest version from system

#[derive(Copy, Clone)]
enum BuildType {
    // CMAKE = 0,
    // MAKE,
    Shell,
    CMD,
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
    bool,                /* needed to run in Unix environment (will be used for windows) */
);

#[allow(clippy::cognitive_complexity)]
fn main() {
    let mut configure_flags: String = " ".to_string();
    let target_os_ffmpeg: &str;
    let target_os_live555: &str;
    let target_cpu_live555 = "amd64"; // TODO: change to sepecific CPU depending on current CPU
    let mut changes_live555 = " ".to_string();
    let target_os =
        std::env::var("CARGO_CFG_TARGET_OS").expect("Build failed: could not get target OS");
    let target_arch = std::env::consts::ARCH;
    let mut shell_type = BuildType::Shell; // default to shell script
    let mut prefix_lib = "";
    let mut pre_command = "".to_string();
    let mut post_command = "".to_string();
    let mut library_prefix_path = "/usr/local/";

    let current_dir_path = std::env::current_dir().expect("could not get current directory");

    let ev_pkg_config_path = option_env!("PKG_CONFIG_PATH");

    if target_os == "windows" {
        if ev_pkg_config_path.is_none() {
            panic!("$PKG_CONFIG_PATH enviroment variable was not set.");
        }

        shell_type = BuildType::CMD;
        prefix_lib = "lib";
        target_os_live555 = "genWindowsMakefiles.cmd";
        configure_flags = format!("{} --toolchain=msvc", configure_flags);
        if target_arch == "x86_64" {
            target_os_ffmpeg = "win64";
        } else {
            target_os_ffmpeg = "win32";
        }
    } else if target_os == "macos" {
        if ev_pkg_config_path.is_none() {
            panic!("$PKG_CONFIG_PATH enviroment variable was not set.");
        }

        shell_type = BuildType::Shell;
        target_os_ffmpeg = "darwin";
        target_os_live555 = "./genMakefiles macosx-bigsur";
    } else if target_os == "linux" {
        shell_type = BuildType::Shell;
        target_os_ffmpeg = "linux";
        target_os_live555 = "./genMakefiles linux-64bit";
    } else {
        panic!("build for target_os not defined.");
    }

    //get current build profile
    let profile_str = std::env::var("PROFILE").expect("Build failed: could not get PROFILE");

    if profile_str == "debug" {
        configure_flags = format!("{} --enable-debug", configure_flags);

        if target_os == "windows" {
            configure_flags = format!(
                "{} --extra-cflags=-MDd --extra-ldflags=/NODEFAULTLIB:libcmt",
                configure_flags
            );
        };
    }

    //get the current path
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

    if target_os == "windows" {
        let current_dir_cloned = current_dir_path.clone();
        let current_dir = current_dir_cloned
            .into_os_string()
            .into_string()
            .expect("could not get a &str from current directory");

        pre_command = [
            format!("\"C:\\Program Files\\Git\\bin\\sh.exe\" -c "),
            "\"".to_string(),
            format!(
                "cd {}/deps/{}/ && ",
                current_dir.replace("\\", "/"),
                "live555"
            ),
        ]
        .to_vec()
        .join(" ");

        let replaced_content = format!("\ncall attrib -r {}/win32config\n\
        echo NODEBUG=1 > win32config\n\
        echo TARGETOS = WINNT >> win32config\n\
        echo UI_OPTS =       $(guilflags) $(guilibsdll) >> win32config\n\
        echo CONSOLE_UI_OPTS =   $(conlflags) $(conlibsdll) >> win32config\n\
        echo CPU={} >> win32config\n\
        echo COMPILE_OPTS =      $(INCLUDES) $(cdebug) $(cflags) $(cvarsdll) -I. /EHsc /O2 /MD /GS /D \"{}\" /Oy- /Oi /D \"NDEBUG\" /D \"NO_GETIFADDRS\" -I\"{}\" >> win32config\n\
        echo C =         c >> win32config\n\
        echo C_COMPILER =        cl >> win32config\n\
        echo C_FLAGS =       $(COMPILE_OPTS) >> win32config\n\
        echo CPP =           cpp >> win32config\n\
        echo CPLUSPLUS_COMPILER =    $(C_COMPILER) >> win32config\n\
        echo CPLUSPLUS_FLAGS =   $(COMPILE_OPTS) >> win32config\n\
        echo OBJ =           obj >> win32config\n\
        echo LINK =              link -out: >> win32config\n\
        echo LIBRARY_LINK =      lib -out: >> win32config\n\
        echo LINK_OPTS_0 =       $(linkdebug) ws2_32.lib /NXCOMPAT >> win32config\n\
        echo LIBRARY_LINK_OPTS = >> win32config\n\
        echo LINK_OPTS =     $(LINK_OPTS_0) $(UI_OPTS) >> win32config\n\
        echo CONSOLE_LINK_OPTS = $(LINK_OPTS_0) $(CONSOLE_UI_OPTS) >> win32config\n\
        echo SERVICE_LINK_OPTS =     kernel32.lib advapi32.lib shell32.lib ws2_32.lib -subsystem:console,$(APPVER) >> win32config\n\
        echo LIB_SUFFIX =        lib >> win32config\n\
        echo LIBS_FOR_CONSOLE_APPLICATION = >> win32config\n\
        echo LIBS_FOR_GUI_APPLICATION = >> win32config\n\
        echo MULTIMEDIA_LIBS =   winmm.lib >> win32config\n\
        echo EXE =           .exe >> win32config\n\
        echo PLATFORM = Windows >> win32config\n\
        echo rc32 = rc.exe >> win32config\n\
        echo .rc.res: >> win32config\n\
        echo    $(rc32) $^< >> win32config\n", format(format_args!(
            "{}/deps/{}/",
            current_dir.replace("\\", "/"),
            "live555"
        )),target_cpu_live555, target_os_ffmpeg, "C:/Program Files/OpenSSL-Win64/include").to_string();

        changes_live555 = [
            format(format_args!(
                "{}call {}",
                replaced_content, target_os_live555
            )),
            "\n cd liveMedia\n".to_string(),
            "nmake -f liveMedia.mak\n".to_string(),
            "cd ..\\groupsock\n".to_string(),
            "nmake -f groupsock.mak\n".to_string(),
            "cd ..\\UsageEnvironment\n".to_string(),
            "nmake -f UsageEnvironment.mak\n".to_string(),
            "cd ..\\BasicUsageEnvironment\n".to_string(),
            "nmake -f BasicUsageEnvironment.mak\n".to_string(),
            format(format_args!(
                "mkdir \"{}/deps/live555/build/{}{}/lib\"\n",current_dir.replace("\\", "/"), build_profile, library_prefix_path
            )),
            format(format_args!(
                "copy \"{}\\deps\\live555\\BasicUsageEnvironment\\libBasicUsageEnvironment.lib\" \"{}/deps/live555/build/{}{}/lib\"\n",
                 current_dir,current_dir, build_profile, library_prefix_path
            )),
            format(format_args!(
                "copy \"{}\\deps\\live555\\groupsock\\libgroupsock.lib\" \"{}/deps/live555/build/{}{}/lib\"\n",
                 current_dir,current_dir, build_profile, library_prefix_path
            )),
            format(format_args!(
                "copy \"{}\\deps\\live555\\liveMedia\\libliveMedia.lib\" \"{}/deps/live555/build/{}{}/lib\"\n",
                 current_dir,current_dir, build_profile, library_prefix_path
            )),
            format(format_args!(
                "copy \"{}\\deps\\live555\\UsageEnvironment\\libUsageEnvironment.lib\" \"{}/deps/live555/build/{}{}/lib\"\n",
                 current_dir,current_dir, build_profile, library_prefix_path
            )),
        ]
        .to_vec()
        .join(" ")
        .to_string();

        post_command = format!("\" {} ", changes_live555);
    } else {
        post_command = format!(
            "&& {} && make clean && make && make install DESTDIR={}/deps/{}/build/{}",
            target_os_live555, current_dir, "live555", build_profile
        );
    }

    let mut git_sources: HashMap<&str, BuildConfig> = HashMap::new();
    if target_os == "macos" || target_os == "windows" {
        let _r = git_sources.insert(
            "ffmpeg",
            (
                shell_type,
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
                    format(format_args!(
                        "cd {}/deps/{}/ && ",
                        current_dir.replace("\\", "/"),
                        "ffmpeg"
                    )),
                    "./configure".to_string(),
                    "--disable-x86asm".to_string(),
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
                    format(format_args!(
                        "--prefix={}/deps/{}/build/{}",
                        current_dir.replace("\\", "/"),
                        "ffmpeg",
                        build_profile
                    )),
                    format!("--target-os={} ", target_os_ffmpeg),
                    format!("--arch={} ", target_arch),
                    format!("{} ", configure_flags),
                    "&& make clean".to_string(),
                    "&& make".to_string(),
                    "&& make install ".to_string(),
                ]
                .to_vec(),
                true,
            ),
        );
    }

    let _r = git_sources.insert(
        "live555",
        (
            shell_type,
            "",
            "",
            [].to_vec(),
            false,
            "src/rtsp/server/cxx/",
            "src/rtsp/server/rtsp_server_live555.rs",
            library_prefix_path,
            [
                (format(format_args!("{}BasicUsageEnvironment",prefix_lib)), true),
                (format(format_args!("{}groupsock",prefix_lib)), true),
                (format(format_args!("{}liveMedia",prefix_lib)), true),
                (format(format_args!("{}UsageEnvironment",prefix_lib)), true),
            ].to_vec(),
            [
                pre_command,
                "curl http://www.live555.com/liveMedia/public/live555-latest.tar.gz > ./live555-latest.tar.gz && ".to_string(),
                "tar -xzvf live555-latest.tar.gz --strip-components 1 ".to_string(),
                post_command
            ]
            .to_vec(),
            false,
        ),
    );

    // make sure set the necessery enviroment variables for OSX
    if target_os == "macos" {
        if MACOSX_DEPLOYMENT_TARGET.is_empty() {
            let file = std::fs::read_to_string("/System/Library/CoreServices/SystemVersion.plist")
                .expect("could not read SystemVersion.plist");
            let cur = std::io::Cursor::new(file.as_bytes());
            let v = plist::Value::from_reader(cur)
                .expect("could not get value from SystemVersion.plist");

            let version = v
                .as_dictionary()
                .and_then(|d| d.get("ProductVersion")?.as_string())
                .expect("SystemVersion.plist is not a dictionary");
            std::env::set_var("MACOSX_DEPLOYMENT_TARGET", version);
        } else {
            std::env::set_var("MACOSX_DEPLOYMENT_TARGET", MACOSX_DEPLOYMENT_TARGET);
        }
    }

    // create vectors for storing rust & cxx sources
    let mut rust_srcs: Vec<String> = Vec::new();
    let mut cxx_srcs: Vec<String> = Vec::new();
    let mut include_srcs: Vec<String> = Vec::new();
    let mut lib_paths: HashMap<String, (String, bool)> = HashMap::new();
    let mut lib_deps = Vec::<(String, bool)>::new();

    // create deps folder
    make_folder(&format!("{}/deps/", current_dir));

    // iterate over git repositories
    for (k, mut v) in git_sources {
        //store deps libraries
        lib_deps.append(&mut v.8);

        let prefix_path = v.7;

        //git project just opened
        let build_dir_lib = format!(
            "{}/deps/{}/build/{}{}/lib",
            current_dir, k, build_profile, prefix_path
        );

        println!("cargo:rustc-link-search=native={}", build_dir_lib);

        //check git project already exists
        let git_repo_path = format!("{}/deps/{}", current_dir, k);
        let mut ret = Repository::open(git_repo_path.clone()).map(|_r| {
            //if build folder does not exist, we should build it again
            !Path::new(&build_dir_lib).exists()
        });

        if ret.is_err() {
            //we should clone the repo again
            let url = v.1;
            ret = if url.is_empty() {
                //those projects which do not have git repos
                Ok(!Path::new(&build_dir_lib).exists())
            } else {
                Repository::clone_recurse(url, git_repo_path.clone()).map(|_repo| {
                    //Time to build it
                    true
                })
            };
        }

        let mut shell_type = "sh";

        match ret {
            Ok(build) => {
                if build {
                    match v.0 {
                        BuildType::Shell => {
                            shell_type = "sh";
                        }
                        BuildType::CMD => {
                            shell_type = "cmd";
                        }
                    }

                    let shell_cmd = &v.9.join(" ");
                    build_shell(
                        shell_type,
                        build_profile,
                        &opt_level_str,
                        &git_repo_path,
                        &v,
                        shell_cmd,
                        v.10,
                    );
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
                let _r = lib_paths.insert(k.to_string(), (path_to_lib.to_string(), link_static));
            }
            Err(e) => {
                panic!("Build failed: error {:?}", e);
            }
        }
    }

    // build cxx lib
    let mut build_cxx = cxx_build::bridges(rust_srcs);
    //build_cxx.flag_if_supported("/std:c++latest");
    let _r = build_cxx.flag_if_supported("-std=c++17");
    let _r = build_cxx.flag_if_supported("-std=c17");
    let _r = build_cxx.flag_if_supported("-Wall");
    let _r = build_cxx.flag_if_supported("-fPIC");

    //add DEBUG or NDEBUG flags
    if profile_str == "debug" {
        let _r = build_cxx.define("DEBUG", "DEBUG");
    } else {
        let _r = build_cxx.define("NDEBUG", "NDEBUG");
    }

    //add necessery includes/libs or defines based on target OS
    match target_os.as_ref() {
        "macos" | "linux" => {
            let _r = build_cxx.include(format!("{}include/", library_prefix_path));
        }
        "windows" => {
            let _r = build_cxx.define("WIN32", "WIN32");
            let _r = build_cxx.define("_WINDOWS", "_WINDOWS");

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
        let _r = std::fs::read_dir(&iter).map(|paths| {
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
                                || ext == "cxx"
                                || ext == "c++"
                            {
                                let _r = build_cxx.file(Path::new(file_path_str));
                            }
                        }
                    }
                    Err(_e) => {}
                }
            }
        });
    }

    //include all includes from CMAKEs
    for iter in include_srcs {
        println!("cargo:rerun-if-changed={}", iter);
        let _r = build_cxx.include(Path::new(&iter));
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
    build_cxx.compile("wolf_stream_cxx");
}

fn build_shell(
    p_shell_type: &str,
    p_make_build_profile: &str,
    p_make_opt_level: &str,
    p_git_repo_path: &str,
    p_value: &BuildConfig,
    p_shell_command: &str,
    p_need_unix_env: bool,
) -> std::io::Result<()> {
    //get path to make file
    let shell_script_parent_path = format!("{}/{}", p_git_repo_path, p_value.2);
    //check build folder
    let build_folder_str = format!("{}build", shell_script_parent_path);
    //check install folder
    let install_folder_str = format!("{}/{}", build_folder_str, p_make_build_profile);

    make_folder(&shell_script_parent_path);
    make_folder(&build_folder_str);
    make_folder(&install_folder_str);

    let mut shell_command = " ".to_string();

    //get the current path
    let current_dir_path = std::env::current_dir().expect("could not get current directory");
    let batch_path = current_dir_path
        .into_os_string()
        .into_string()
        .expect("could not get a &str from current directory");

    if cfg!(windows) {
        let mut vs_path = find_msvc_path(&batch_path, p_shell_type)?;

        if p_need_unix_env {
            //run in Unix environment (i.e. mingw64, mingw32, msys2, ...)
            shell_command =
                find_unix_shell_path(&shell_script_parent_path, vs_path, p_shell_command);
        } else {
            shell_command = format!(
                "call cd /D {}\n call {}\n call {} ",
                &shell_script_parent_path, vs_path, p_shell_command
            );
        }

        //write command to a batch file (running the command directly through cmd
        //does not work in windows. So we write the command to a batch file and then execute this batch file)
        let mut command_file = batch_path.clone();
        command_file.push_str("/command.bat");

        write_to_file(&command_file, &shell_command)?;

        //write the path of command batch file to shell_command
        shell_command = format!("{}", command_file);
    } else {
        shell_command = format!("cd {} && {}", &shell_script_parent_path, &p_shell_command);
    };

    //configure cmake
    execute_command(
        p_shell_type,
        shell_command.as_str(),
        shell_script_parent_path.as_str(),
    );

    if cfg!(windows) {
        // remove the command batch file
        std::fs::remove_file(format!("{}/command.bat", batch_path.as_str()))?;
    }

    Ok(())
}

fn write_to_file(p_file_path: &String, p_buffer: &str) -> std::io::Result<()> {
    let mut file = std::fs::File::create(p_file_path.as_str())?;
    file.write_all(p_buffer.as_bytes())?;
    Ok(())
}

fn find_unix_shell_path(
    p_shell_script_parent_path: &String,
    p_vs_path: String,
    p_shell_command: &str,
) -> String {
    // build for current arch
    let target_arch = std::env::consts::ARCH;
    let mut arch = "mingw64".to_string(); //default to x64

    if target_arch == "x86_64" {
        arch = "mingw64".to_string();
    } else if target_arch == "x86" {
        arch = "mingw32".to_string();
    }

    let msys_shell = [
        "C:/tools/msys64/msys2_shell.cmd".to_string(),
        "-use-full-path".to_string(),
        "-no-start".to_string(),
        "-here".to_string(),
        "-defterm".to_string(),
        format!("-{}", arch).to_string(),
        "-c ".to_string(),
    ]
    .to_vec();

    let shell_command = format!(
        "cd /D {} && {} && {} \"{}\"",
        &p_shell_script_parent_path,
        p_vs_path,
        msys_shell.join(" "),
        p_shell_command
    )
    .to_string();

    shell_command
}

fn find_msvc_path(p_batch_path: &String, p_shell_type: &str) -> std::io::Result<String> {
    let mut shell_command = " ".to_string(); // default to shell script

    let mut batch_file = p_batch_path.clone();
    batch_file.push_str("/vcvarsall_path.bat");

    let mut buffer = "@echo off\n \
                        setlocal\n \
                        set vec[0]=10\n \
                        set vec[1]=11\n \
                        set vec[2]=12\n \
                        set vec[3]=14\n \
                        set vec[4]=15\n \
                        set vec[5]=16\n \
                        FOR /L %%x IN (0,1,5) DO call :process_testdir %%x VSPATH\n \
                        if \"%VSPATH%\" NEQ \"\" echo \"%VSPATH%VC/vcvarsall.bat\"\n \
                        exit /b\n \
                        :process_testdir\n \
                        set y=%1\n \
                        set n=%%vec[%y%]%%\n \
                        for /f \"tokens=1,2*\" %%a in ('reg query \"HKLM\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VS7\" /v \"%n%.0\" 2^>nul') do set \"localpath=%%c\"\n \
                        if \"%localpath%\" NEQ \"\" set %2=%localpath%\n \
                        goto :eof\n \
                        endlocal         
        ";

    write_to_file(&batch_file, &buffer);

    shell_command =
        format!("{} > {}/output", batch_file.as_str(), p_batch_path.as_str()).to_string();
    execute_command(p_shell_type, shell_command.as_str(), p_batch_path.as_str());

    let mut vs_path = std::fs::read_to_string(format!("{}/output", p_batch_path.as_str()))
        .expect("Something went wrong reading the file");

    if vs_path == "" {
        return Err(Error::new(
            ErrorKind::Other,
            format!("Can not find {}", &shell_command),
        ));
    }

    if vs_path.ends_with('\n') {
        vs_path.pop();
        if vs_path.ends_with('\r') {
            vs_path.pop();
        }
    }

    // build for current arch
    let target_arch = std::env::consts::ARCH;
    if target_arch == "x86_64" {
        vs_path.push_str(r#" x64"#);
    } else if target_arch == "x86" {
        vs_path.push_str(r#" x86"#);
    }

    std::fs::remove_file(format!("{}/output", p_batch_path.as_str()))?;
    std::fs::remove_file(format!("{}/vcvarsall_path.bat", p_batch_path.as_str()))?;
    Ok(vs_path)
}

fn execute_command(p_shell: &str, p_shell_command: &str, p_make_parent_path: &str) {
    let mut param = "-c";

    if cfg!(windows) {
        param = "/C";
    }

    let out = std::process::Command::new(p_shell)
        .current_dir(p_make_parent_path)
        .args([param, p_shell_command])
        .output()
        .expect(
            format!(
                "Build failed: could not build make for {}MakeFile",
                p_make_parent_path
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
        std::fs::create_dir(p).unwrap_or_else(|_| panic!("could not create folder {}", path));
    }
}
