#![allow(unused_mut)]

use std::{path::Path, process::Command};

// TODO: read these params from args
const OSX_DEPLOYMENT_TARGET: &str = "12.0";
const ANDROID_API_LEVEL: i32 = 21;
// https://cmake.org/cmake/help/latest/variable/CMAKE_ANDROID_ARCH_ABI.html
const ANDROID_ARCH_API: &str = "armeabi-v7a";

#[allow(dead_code)]
// https://developer.android.com/ndk/guides/other_build_systems
const ANDROID_NDK_OS_VARIANT: &str = "darwin-x86_64";

// Cmake Compiler
#[cfg(target_family = "windows")]
const CMAKE_C_COMPILER: &str = "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.32.31326/bin/Hostx64/x64/cl.exe";

#[cfg(any(target_family = "unix"))]
const CMAKE_C_COMPILER: &str = "usr/bin/clang";

fn main() {
    // get the current path
    let current_dir_path = std::env::current_dir().expect("could not get current directory");
    let current_dir_path_str = current_dir_path
        .to_str()
        .expect("could not convert current dir path to &str");

    //get current target os
    let target_arch =
        std::env::var("CARGO_CFG_TARGET_ARCH").expect("Build failed: could not get target arch");

    let target_os =
        std::env::var("CARGO_CFG_TARGET_OS").expect("Build failed: could not get target OS");

    if target_os == "macos" {
        std::env::set_var("MACOSX_DEPLOYMENT_TARGET", OSX_DEPLOYMENT_TARGET);
    }

    let wolf_lib_name = if target_os == "windows" {
        "wolf_sys.lib"
    } else {
        "libwolf_sys.a"
    };

    #[cfg(feature = "system_raft")]
    build_protos(current_dir_path, target_os);

    // don't compoile any c++ codes for wasm32
    if target_arch == "wasm32" {
        return;
    }

    // get opt_level
    let opt_level_str = std::env::var("OPT_LEVEL").expect("could not get OPT_LEVEL profile");

    // get current build profile
    let profile_str = std::env::var("PROFILE").expect("could not get PROFILE");
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

    // execute cmake for building deps of wolf_sys
    cmake(&current_dir_path, wolf_lib_name, build_profile, &target_os);

    // compile c/cpp sources and link
    link(current_dir_path_str, build_profile, &target_os);
}

/// # Errors
///
/// Will return `Err` if `proto` does not exist or is invalid.
#[cfg(feature = "system_raft")]
fn build_protos(p_current_dir_path: &str, p_target_os: &str) {
    // compile protos
    let mut build_server_proto = true;
    let build_client_proto = true;
    // we don't want proto server for android and ios clients
    if p_target_os == "android" || p_target_os == "ios" {
        build_server_proto = false;
    }

    let proto_path_include = p_current_dir_path.join("proto");
    let proto_path_include_src = proto_path_include
        .to_str()
        .expect("could not convert include proto path to &str");

    let proto_path = proto_path_include.join("raft.proto");
    let proto_path_str = proto_path
        .to_str()
        .expect("could not convert source proto path to &str");

    protos(
        &[proto_path_str],
        &[proto_path_include_src],
        build_client_proto,
        build_server_proto,
    )
    .expect("couldn't read the protos directory");
}

/// # Errors
///
/// Will return `Err` if `proto` does not exist or is invalid.
#[cfg(feature = "system_raft")]
pub fn protos(
    p_proto_paths: &[&str],
    p_proto_includes: &[&str],
    p_build_client: bool,
    p_build_server: bool,
) -> io::Result<()> {
    tonic_build::configure()
        .build_client(p_build_client)
        .build_server(p_build_server)
        .compile(p_proto_paths, p_proto_includes)
}

/// # Panic
///
/// Will be panic `if cmake failed
pub fn cmake(
    p_current_dir_path_str: &Path,
    p_wolf_sys_file_name: &str,
    p_build_profile: &str,
    p_target_os: &str,
) {
    // get parent path
    let cmake_current_path = p_current_dir_path_str.join("sys/");
    let cmake_current_path_str = cmake_current_path
        .to_str()
        .expect("could not convert cmake current path to &str");

    // get cmake build path
    let install_folder = format!("build/{}", p_build_profile);
    let cmake_build_path = cmake_current_path.join(install_folder);
    let cmake_build_path_str = cmake_build_path
        .to_str()
        .expect("could not convert cmake build path to &str");

    // return if wolf_sys library was found
    let wolf_sys_path = if p_target_os == "windows" {
        cmake_build_path
            .join(p_build_profile)
            .join(p_wolf_sys_file_name)
    } else {
        cmake_build_path.join(p_wolf_sys_file_name)
    };
    if std::path::Path::new(&wolf_sys_path).exists() {
        return;
    }

    #[cfg(not(target_os = "windows"))]
    let build_cmd = "-GNinja";

    #[cfg(target_os = "windows")]
    let build_cmd = "";

    // args
    let mut args = [
        ".".to_owned(),
        "--no-warn-unused-cli".to_owned(),
        "-Wdev".to_owned(),
        "--debug-output".to_owned(),
        "-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE".to_owned(),
        format!("-DCMAKE_C_COMPILER={}", CMAKE_C_COMPILER),
        format!("-DCMAKE_BUILD_TYPE:STRING={}", p_build_profile),
        format!("-B{}", cmake_build_path_str),
        format!("-S{}", cmake_current_path_str),
        build_cmd.to_owned(),
    ]
    .to_vec();

    // set defines
    args.push("-DWOLF_ENABLE_TESTS=OFF".to_owned());

    #[cfg(feature = "system_lz4")]
    args.push("-DWOLF_SYSTEM_LZ4=ON".to_owned());

    #[cfg(feature = "stream_rist")]
    args.push("-DWOLF_STREAM_RIST=ON".to_owned());

    #[cfg(any(feature = "ffmpeg"))]
    args.push("-DWOLF_MEDIA_FFMPEG=ON".to_owned());

    if p_target_os == "android" {
        let android_ndk_home_env =
            std::env::var("ANDROID_NDK_HOME").expect("could not get ANDROID_NDK_HOME");
        args.push(format!(
            "-DCMAKE_TOOLCHAIN_FILE={}/build/cmake/android.toolchain.cmake",
            android_ndk_home_env
        ));
        args.push(format!("-DANDROID_ABI={}", ANDROID_ARCH_API));
        args.push(format!("-DANDROID_NDK={}", android_ndk_home_env));
        args.push(format!("-DANDROID_PLATFORM=android-{}", ANDROID_API_LEVEL));
        args.push(format!("-DCMAKE_ANDROID_ARCH_ABI={}", ANDROID_ARCH_API));
        args.push(format!("-DCMAKE_ANDROID_NDK={}", android_ndk_home_env));
        args.push("-DCMAKE_EXPORT_COMPILE_COMMANDS=ON".to_owned());
        args.push("-DCMAKE_SYSTEM_NAME=Android".to_owned());
        args.push(format!("-DCMAKE_SYSTEM_VERSION={}", ANDROID_API_LEVEL));
    }

    // configure
    let mut out = Command::new("cmake")
        .current_dir(&cmake_current_path)
        .args(args)
        .output()
        .expect("could not configure cmake of wolf/sys");

    assert!(
        out.status.success(),
        "CMake project was not configured successfully\r\nstdout:{:?}\r\nstderr:{:?}",
        std::str::from_utf8(&out.stdout),
        std::str::from_utf8(&out.stderr),
    );

    // build cmake
    if cfg!(target_os = "windows") {
        out = Command::new("cmake")
            .current_dir(&cmake_build_path)
            .args(["--build", ".", "--parallel 8"])
            .output()
            .expect("could not build cmake of wolf/sys");
    } else {
        out = Command::new("ninja")
            .current_dir(&cmake_build_path)
            .output()
            .expect("could not build cmake of wolf/sys");
    }

    assert!(
        out.status.success(),
        "CMake Build failed for {}CMakeLists.txt\r\nstdout:{:?}\r\nstderr:{:?}",
        cmake_current_path_str,
        std::str::from_utf8(&out.stdout),
        std::str::from_utf8(&out.stderr),
    );
}

/// # Panic
///
/// Will be panic `if link failed
fn link(p_current_dir_path_str: &str, p_build_profile: &str, p_target_os: &str) {
    if p_target_os == "windows" {
        if p_build_profile == "Debug" {
            println!("cargo:rustc-link-lib=msvcrtd");
        } else {
            println!("cargo:rustc-link-lib=msvcrt");
        }
        println!("cargo:rustc-link-lib=dylib=Shell32");
        println!("cargo:rustc-link-lib=dylib=Rpcrt4");
        println!("cargo:rustc-link-lib=dylib=Mswsock");
    }

    let sys_build_dir = format!("{}/sys/build/{}", p_current_dir_path_str, p_build_profile);

    if cfg!(target_family = "unix") {
        println!("cargo:rustc-link-search=native=/usr/lib");
        println!("cargo:rustc-link-lib=dylib=c++");
    }

    let lib_path = if p_target_os == "windows" {
        format!("{}/{}", sys_build_dir, p_build_profile)
    } else {
        sys_build_dir
    };
    println!("cargo:rustc-link-search=native={}", lib_path);
    println!("cargo:rustc-link-lib=dylib=wolf_sys");

    // copy to target and deps folder
    copy_shared_libs(lib_path);
}

#[cfg(target_os = "windows")]
fn copy_shared_libs(p_lib_path: String) {
    let out_dir = std::env::var("OUT_DIR").unwrap();

    let out_path = std::path::Path::new(&out_dir).join("../../..");
    let deps_path = out_path.join("deps");
    let names = [
        "wolf_sys.dll",
        "wolf_sys.exp",
        "wolf_sys.lib",
        "wolf_sys.pdb",
    ];
    for name in names {
        let file = format!("{}/{}", p_lib_path, name);
        let _ = std::fs::copy(&file, deps_path.join(name));
        let _ = std::fs::copy(&file, out_path.join(name));
    }
}
