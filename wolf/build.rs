#![allow(unused_mut)]

use std::{
    fs,
    io::{self, Write},
    path::Path,
    process::Command,
};

const MACOSX_DEPLOYMENT_TARGET: &str = "12.0";

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
        "wolf_sys.lib"
    } else {
        "libwolf_sys.a"
    };

    let proto_path_include = current_dir_path.join("proto");
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

    // execute cmake of wolf_sys
    cmake(&current_dir_path, wolf_lib_name, build_profile, &target_os);

    // link to wolf_sys
    link(current_dir_path_str, build_profile, &target_os);

    // create bindgens from wolf_sys
    bindgens(current_dir_path_str);
}

/// # Errors
///
/// Will return `Err` if `proto` does not exist or is invalid.
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

    // set defines
    #[cfg(feature = "system_lz4")]
    args.push("-DWOLF_ENABLE_LZ4=ON");

    #[cfg(feature = "stream_rist")]
    args.push("-DWOLF_ENABLE_RIST=ON");

    // configure
    let mut out = Command::new("cmake")
        .current_dir(&cmake_current_path)
        .args(args)
        .output()
        .expect("could not configure cmake of wolf/sys");

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
        .expect("could not build cmake of wolf/sys");

    assert!(
        out.status.success(),
        "CMake Build failed for {}CMakeLists.txt",
        cmake_current_path_str
    );
}

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

    let sys_build_dir = format!("{}/sys/build/{}", p_current_dir_path_str, p_build_profile,);
    let sys_deps_dir = format!("{}/_deps", sys_build_dir,);

    struct Dep {
        search_path: String,
        lib_name: String,
    }
    let mut deps = Vec::new();
    deps.push(Dep {
        search_path: format!("{}", sys_build_dir),
        lib_name: format!("wolf_sys"),
    });
    deps.push(Dep {
        search_path: format!("{}/mimalloc-static-build/", sys_deps_dir),
        lib_name: format!("mimalloc-{}", p_build_profile.to_lowercase()),
    });
    if cfg!(feature = "system_lz4") {
        deps.push(Dep {
            search_path: format!("{}/lz4-build/", sys_deps_dir),
            lib_name: "lz4".to_owned(),
        });
    }

    for dep in deps {
        println!("cargo:rustc-link-search=native={}", dep.search_path);
        println!("cargo:rustc-link-lib=static={}", dep.lib_name);
    }
}

fn bindgens(p_current_dir_path_str: &str) {
    // mod
    let mut mod_rs = "#![allow(non_upper_case_globals)]\r\n#![allow(non_camel_case_types)]\r\n#![allow(non_snake_case)]\r\n".to_owned();

    // remove all rust sources from ffi
    let ffi_path = Path::new(p_current_dir_path_str).join("src/ffi");
    if ffi_path.exists() {
        fs::remove_dir_all(ffi_path.clone()).expect("could not remove sys ffi");
    }

    // create ffi
    fs::create_dir(ffi_path.clone()).expect("could not create sys ffi");

    struct BindgenPipeline<'a> {
        rust_src: &'a str,
        header_src: &'a str,
        c_src: &'a str,
        allowlist_types: Vec<&'a str>,
        allowlist_funcs: Vec<&'a str>,
    }
    let mut srcs = Vec::new();

    // set defines
    if cfg!(feature = "system_lz4") {
        srcs.push(BindgenPipeline {
            rust_src: "src/ffi/lz4.rs",
            header_src: "sys/compression/lz4.h",
            c_src: "sys/compression/lz4.cpp",
            allowlist_types: vec![],
            allowlist_funcs: vec!["compress"],
        });
        mod_rs += "#[cfg(feature = \"system_lz4\")]\r\npub mod lz4;\r\n";
    }

    let include_path = format!("-I{}/sys", p_current_dir_path_str);
    for src in srcs {
        // generate bindgen
        println!("cargo:rerun-if-changed={}", src.header_src);
        println!("cargo:rerun-if-changed={}", src.c_src);

        let mut builder = bindgen::Builder::default()
            // The input header we would like to generate
            // bindings for.
            .header(src.header_src)
            .layout_tests(false)
            .clang_args(&[&include_path, "-std=c18"]);

        for t in src.allowlist_types {
            builder = builder.allowlist_type(t);
        }
        for f in src.allowlist_funcs {
            builder = builder.allowlist_function(f);
        }

        let bindings = builder
            // Tell cargo to invalidate the built crate whenever any of the
            // included header files changed.
            .parse_callbacks(Box::new(bindgen::CargoCallbacks))
            // Finish the builder and generate the bindings.
            .generate()
            // Unwrap the Result and panic on failure.
            .unwrap_or_else(|_| panic!("couldn't build bindings for {}", src.header_src));

        // write the bindings to the file.
        let out_path = format!("{}/{}", p_current_dir_path_str, src.rust_src);
        bindings
            .write_to_file(Path::new(&out_path))
            .unwrap_or_else(|e| {
                panic!("couldn't write bindings for {} because {}", src.rust_src, e)
            });
    }

    // handle mod.rs
    let ffi_mod_path = ffi_path.join("mod.rs");
    let mut file = fs::File::create(ffi_mod_path).expect("couldn't create ffi/mod.rs");
    file.write_all(mod_rs.as_bytes())
        .expect("couldn't write to ffi/mod.rs");
}
