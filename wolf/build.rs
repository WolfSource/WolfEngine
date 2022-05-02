#![allow(unused_mut)]
#![allow(unused_imports)]
use std::{env, fs, io, path::Path, process::Command};

const MACOSX_DEPLOYMENT_TARGET: &str = "12.0";

fn main() {
    // get the current path
    let current_dir_path = std::env::current_dir().expect("could not get current directory");
    let current_dir_path_str = current_dir_path
        .to_str()
        .expect("could not convert current dir path to &str");

    //get current target os
    let target_os =
        std::env::var("CARGO_CFG_TARGET_OS").expect("Build failed: could not get target OS");

    if target_os == "macos" {
        std::env::set_var("MACOSX_DEPLOYMENT_TARGET", MACOSX_DEPLOYMENT_TARGET);
    }

    // compile protos
    let mut build_server_proto = true;
    let build_client_proto = true;
    // we don't want proto server for android and ios clients
    if target_os == "android" || target_os == "ios" {
        build_server_proto = false;
    }

    let wolf_lib_name = if target_os == "windows" {
        "wolf_cxx.dll"
    } else if target_os == "macos" {
        "libwolf_cxx.dylib"
    } else {
        "libwolf_cxx.so"
    };

    let proto_path_include = current_dir_path.join("proto");
    let proto_path_include_src = proto_path_include
        .to_str()
        .expect("could not convert include proto path to &str");

    let proto_path = proto_path_include.join("raft.proto");
    let proto_path_str = proto_path
        .to_str()
        .expect("could not convert source proto path to &str");

    compile_protos(
        &[proto_path_str],
        &[proto_path_include_src],
        build_client_proto,
        build_server_proto,
    )
    .expect("couldn't read the protos directory");

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

    // execute cmake of wolf_cxx
    build_cmake(&current_dir_path, wolf_lib_name, build_profile, &target_os);

    // build cxx
    build_cxx(
        current_dir_path_str,
        wolf_lib_name,
        build_profile,
        &target_os,
    );
}

/// # Errors
///
/// Will return `Err` if `proto` does not exist or is invalid.
pub fn compile_protos(
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
pub fn build_cmake(
    p_current_path: &Path,
    p_wolf_cxx_file_name: &str,
    p_build_profile: &str,
    p_target_os: &str,
) {
    // get parent path
    let cmake_current_path = p_current_path.join("cxx/");
    let cmake_current_path_str = cmake_current_path
        .to_str()
        .expect("could not convert cmake current path to &str");

    // get cmake build path
    let install_folder = format!("build/{}", p_build_profile);
    let cmake_build_path = cmake_current_path.join(install_folder);
    let cmake_build_path_str = cmake_build_path
        .to_str()
        .expect("could not convert cmake build path to &str");

    // return if wolf_cxx library was found
    let wolf_cxx_path = if p_target_os == "windows" {
        cmake_build_path
            .join(p_build_profile)
            .join(p_wolf_cxx_file_name)
    } else {
        cmake_build_path.join(p_wolf_cxx_file_name)
    };
    if std::path::Path::new(&wolf_cxx_path).exists() {
        return;
    }

    // args
    let b_arg = format!("-B{}", cmake_build_path_str);
    let s_arg = format!("-S{}", cmake_current_path_str);
    let type_arg = format!("-DCMAKE_BUILD_TYPE:STRING={}", p_build_profile);
    let mut args = [
        ".",
        "--no-warn-unused-cli",
        "-Wdev",
        "--debug-output",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE",
        &b_arg,
        &s_arg,
        &type_arg,
    ]
    .to_vec();

    cfg_if::cfg_if! {
        if #[cfg(feature = "lz4")] {
            args.push("-DWOLF_ENABLE_LZ4=ON");
        }
    }

    // configure
    let mut out = Command::new("cmake")
        .current_dir(&cmake_current_path)
        .args(args)
        .output()
        .expect("could not configure cmake of wolf/cxx");

    assert!(
        out.status.success(),
        "CMake project was not configured successfully because: {:?}",
        std::str::from_utf8(&out.stderr)
    );

    // build cmake
    out = Command::new("cmake")
        .current_dir(&cmake_current_path)
        .args([
            "--build",
            cmake_build_path_str,
            "--config",
            p_build_profile,
            "--parallel",
            "8",
        ])
        .output()
        .expect("could not build cmake of wolf/cxx");

    assert!(
        out.status.success(),
        "CMake Build failed for {} because: {:?}",
        cmake_current_path_str,
        std::str::from_utf8(&out.stderr)
    );
}

fn build_cxx(
    p_current_dir_path_str: &str,
    p_wolf_cxx_file_name: &str,
    p_build_profile: &str,
    p_target_os: &str,
) {
    let post_lib_path = if p_target_os == "windows" {
        if p_build_profile == "Debug" {
            println!("cargo:rustc-link-lib=msvcrtd");
        } else {
            println!("cargo:rustc-link-lib=msvcrt");
        }
        println!("cargo:rustc-link-lib=dylib=Shell32");
        println!("cargo:rustc-link-lib=dylib=Rpcrt4");
        println!("cargo:rustc-link-lib=dylib=Mswsock");

        p_build_profile
    } else {
        ""
    };

    // includes + rust & cpp sources
    let gsl_include = format!("cxx/build/{}/_deps/gsl-src/include", p_build_profile);
    let mimalloc_include = format!("cxx/build/{}/_deps/mimalloc-src/include", p_build_profile);

    let mut rusts = Vec::<&str>::new();
    let mut cpps = Vec::<&str>::new();
    let mut includes = vec!["./", "cxx/", &gsl_include, &mimalloc_include];

    println!("cargo:rustc-link-search=native=/usr/lib/");

    cfg_if::cfg_if! {
            if #[cfg(feature = "lz4")] {
                // update rust & cpp sources
                rusts.push("src/system/compression/lz4.rs");
                cpps.push("src/system/compression/cxx/lz4/LZ4.cpp");

                // lz4 includes
                let lz4_include = format!("cxx/build/{}/_deps/lz4-src/lib", p_build_profile);
                includes.push(&lz4_include);

                let dir = "src/system/compression/cxx/lz4";
                println!("cargo:rerun-if-changed={}/LZ4.hpp", dir);
                println!("cargo:rerun-if-changed={}/LZ4.cpp", dir);

            // link to lz4 library
            println!(
                "cargo:rustc-link-search=native={}/cxx/build/{}/_deps/lz4-build/{}",
                p_current_dir_path_str, p_build_profile, post_lib_path
            );
            println!("cargo:rustc-link-lib=static=lz4");
        }
    }

    let lib_dir = format!(
        "{}/cxx/build/{}/{}",
        p_current_dir_path_str, p_build_profile, post_lib_path
    );
    if !rusts.is_empty() {
        cxx_build::bridges(rusts) // returns a cc::Build
            .files(cpps)
            .includes(includes)
            .flag_if_supported("-std=c++20")
            .flag_if_supported("-fPIC")
            .flag_if_supported("-Wall")
            .compile("wolf_cxx_bridge");

        // link to wolf_cxx library
        println!("cargo:rustc-link-search=native={}", &lib_dir);
        println!("cargo:rustc-link-lib=dylib=wolf_cxx");
    }

    // copy the dynamic library
    let from_path = Path::new(&lib_dir).join(p_wolf_cxx_file_name);
    let out = env::var("OUT_DIR").unwrap();
    let to_path = Path::new(&out).join("../../..").join(p_wolf_cxx_file_name);
    fs::copy(from_path, to_path).unwrap();
}
