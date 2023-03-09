#![allow(unreachable_code)]
#![allow(unused_mut)]
#![allow(unused_variables)]

use std::{path::Path, process::Command};
use tonic_build as _;

// TODO: read these params from args
const OSX_DEPLOYMENT_TARGET: &str = "12.0";
const ANDROID_API_LEVEL: i32 = 21;
// https://cmake.org/cmake/help/latest/variable/CMAKE_ANDROID_ARCH_ABI.html
const ANDROID_ARCH_API: &str = "armeabi-v7a";

#[allow(dead_code)]
// https://developer.android.com/ndk/guides/other_build_systems
const ANDROID_NDK_OS_VARIANT: &str = "darwin-x86_64";

// CMake Compiler
#[cfg(target_os = "windows")]
const CMAKE_C_COMPILER: &str = "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.32.31326/bin/Hostx64/x64/cl.exe";

#[cfg(any(target_os = "macos", target_os = "ios"))]
const CMAKE_C_COMPILER: &str = "/Library/Developer/CommandLineTools/usr/bin/clang";

#[cfg(any(target_os = "linux"))]
const CMAKE_C_COMPILER: &str = "/usr/bin/clang";

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

    #[cfg(feature = "system_raft")]
    build_protos(&current_dir_path, &target_os);

    // don't compile any c++ codes for wasm32
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

    // execute cmake for building deps
    cmake(&current_dir_path, build_profile, &target_os);

    // compile c/cpp sources and link
    link(current_dir_path_str, build_profile, &target_os);

    // generate bindings
    bindgens(current_dir_path_str, build_profile);
}

/// # Errors
///
/// Will return `Err` if `proto` does not exist or is invalid.
#[cfg(feature = "system_raft")]
fn build_protos(p_current_dir_path: &std::path::Path, p_target_os: &str) {
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
) -> std::io::Result<()> {
    tonic_build::configure()
        .protoc_arg("--experimental_allow_proto3_optional")
        .build_client(p_build_client)
        .build_server(p_build_server)
        .compile(p_proto_paths, p_proto_includes)
}

/// # Panics
///
/// Will be panic if `CMake` process will be failed
pub fn cmake(p_current_dir_path_str: &Path, p_build_profile: &str, p_target_os: &str) {
    // get parent path
    let cmake_current_path = p_current_dir_path_str.join("sys/");
    let cmake_current_path_str = cmake_current_path
        .to_str()
        .expect("could not convert cmake current path to &str");

    // get cmake build path
    let install_folder = format!("build/{p_build_profile}");
    let cmake_build_path = cmake_current_path.join(install_folder);
    let cmake_build_path_str = cmake_build_path
        .to_str()
        .expect("could not convert cmake build path to &str");

    // return if wolf_sys library was found
    let wolf_sys_path = if p_target_os == "windows" {
        cmake_build_path.join(p_build_profile).join("wolf_sys.lib")
    } else {
        cmake_build_path.join("libwolf_sys.a")
    };

    if std::path::Path::new(&wolf_sys_path).exists() {
        return;
    }

    let args = get_cmake_defines(
        p_target_os,
        p_build_profile,
        cmake_build_path_str,
        cmake_current_path_str,
    );

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
    out = Command::new("cmake")
        .current_dir(&cmake_build_path)
        .args(["--build", ".", "--parallel 8"])
        .output()
        .expect("could not build cmake of wolf/sys");

    assert!(
        out.status.success(),
        "CMake Build failed for {cmake_current_path_str}CMakeLists.txt\r\nstdout:{:?}\r\nstderr:{:?}",
        std::str::from_utf8(&out.stdout),
        std::str::from_utf8(&out.stderr),
    );
}

fn get_cmake_defines(
    p_target_os: &str,
    p_build_profile: &str,
    p_cmake_build_path: &str,
    p_cmake_current_path: &str,
) -> Vec<String> {
    let build_cmd = "";

    // args
    let mut args = [
        ".".to_owned(),
        "--no-warn-unused-cli".to_owned(),
        "-Wdev".to_owned(),
        "--debug-output".to_owned(),
        "-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE".to_owned(),
        format!("-DCMAKE_C_COMPILER={CMAKE_C_COMPILER}"),
        format!("-DCMAKE_BUILD_TYPE:STRING={p_build_profile}"),
        format!("-B{p_cmake_build_path}"),
        format!("-S{p_cmake_current_path}"),
        build_cmd.to_owned(),
    ]
    .to_vec();

    // set defines
    #[cfg(feature = "system_lz4")]
    args.push("-DWOLF_SYSTEM_LZ4=ON".to_owned());

    #[cfg(feature = "system_lzma")]
    args.push("-DWOLF_SYSTEM_LZMA=ON".to_owned());

    #[cfg(feature = "system_gamepad_virtual")]
    args.push("-DWOLF_SYSTEM_GAMEPAD_VIRTUAL=ON".to_owned());

    #[cfg(feature = "stream_rist")]
    args.push("-DWOLF_STREAM_RIST=ON".to_owned());

    #[cfg(any(feature = "media_ffmpeg"))]
    args.push("-DWOLF_MEDIA_FFMPEG=ON".to_owned());

    #[cfg(any(feature = "media_openal"))]
    args.push("-DWOLF_MEDIA_OPENAL=ON".to_owned());

    if p_target_os == "android" {
        let android_ndk_home_env =
            std::env::var("ANDROID_NDK_HOME").expect("could not get ANDROID_NDK_HOME");
        args.push(format!(
            "-DCMAKE_TOOLCHAIN_FILE={android_ndk_home_env}/build/cmake/android.toolchain.cmake"
        ));
        args.push(format!("-DANDROID_ABI={ANDROID_ARCH_API}"));
        args.push(format!("-DANDROID_NDK={android_ndk_home_env}"));
        args.push(format!("-DANDROID_PLATFORM=android-{ANDROID_API_LEVEL}"));
        args.push(format!("-DCMAKE_ANDROID_ARCH_ABI={ANDROID_ARCH_API}"));
        args.push(format!("-DCMAKE_ANDROID_NDK={android_ndk_home_env}"));
        args.push("-DCMAKE_EXPORT_COMPILE_COMMANDS=ON".to_owned());
        args.push("-DCMAKE_SYSTEM_NAME=Android".to_owned());
        args.push(format!("-DCMAKE_SYSTEM_VERSION={ANDROID_API_LEVEL}"));
    }

    args
}

#[allow(dead_code)]
fn copy_shared_libs(p_lib_path: &str, p_lib_names: &[String]) {
    let out_dir = std::env::var("OUT_DIR").unwrap();

    let out_path = std::path::Path::new(&out_dir).join("../../..");
    let deps_path = out_path.join("deps");

    for name in p_lib_names.iter() {
        let file = format!("{p_lib_path}/{name}");
        let _file1 = std::fs::copy(&file, deps_path.join(name));
        let _file2 = std::fs::copy(&file, out_path.join(name));
    }
}

/// # Panic
///
/// Will be panic if the link process will be failed
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

    if cfg!(target_os = "macos") || cfg!(target_os = "ios") {
        println!("cargo:rustc-link-search=native=/usr/lib");
        println!("cargo:rustc-link-lib=dylib=c++");
    }

    let lib_ext = if p_target_os == "windows" { "lib" } else { "a" };

    // create system build directory
    let sys_build_dir = format!("{p_current_dir_path_str}/sys/build/{p_build_profile}");

    let lib_path = if p_target_os == "windows" {
        format!("{sys_build_dir}/{p_build_profile}")
    } else {
        sys_build_dir
    };
    // println!("cargo:rustc-link-search=native={lib_path}");
    // println!("cargo:rustc-link-lib=static=wolf_sys");

    let mut libs = Vec::<(String, String)>::new();
    #[cfg(feature = "system_lz4")]
    libs.push((
        format!("lz4_static-build/{p_build_profile}"),
        "lz4".to_owned(),
    ));

    // #[cfg(feature = "system_lz4")]
    // libs.push((format!("lz4-build/{p_build_profile}"), "lzma".to_owned()));

    #[cfg(feature = "media_openal")]
    {
        libs.push((
            format!("openal-build/{p_build_profile}"),
            "OpenAL32".to_owned(),
        ));
        copy_openal(p_current_dir_path_str, p_build_profile);
    }

    #[cfg(feature = "stream_rist")]
    libs.push(("librist-build".to_owned(), "rist".to_owned()));

    #[cfg(feature = "system_gamepad_virtual")]
    {
        println!("cargo:rustc-link-lib=Xinput");
        println!("cargo:rustc-link-lib=SetupAPI");
        libs.push((
            format!("vigemclient-build/{p_build_profile}"),
            "ViGEmClient".to_owned(),
        ));
    }

    for lib in libs {
        println!("cargo:rustc-link-search=native={p_current_dir_path_str}/sys/build/{p_build_profile}/_deps/{}", lib.0);
        println!("cargo:rustc-link-lib=static={}", lib.1);
    }

    #[cfg(feature = "media_ffmpeg")]
    copy_ffmpeg(p_current_dir_path_str, p_target_os);
}

#[cfg(feature = "media_ffmpeg")]
fn copy_ffmpeg(p_current_dir_path_str: &str, p_target_os: &str) {
    let ffmpeg_dll_names = [
        "avcodec-59.dll".to_owned(),
        "avdevice-59.dll".to_owned(),
        "avfilter-8.dll".to_owned(),
        "avformat-59.dll".to_owned(),
        "avutil-57.dll".to_owned(),
        "postproc-56.dll".to_owned(),
        "swresample-4.dll".to_owned(),
        "swscale-6.dll".to_owned(),
    ];

    let ffmpeg_bin_path =
        format!("{p_current_dir_path_str}/sys/third_party/ffmpeg/bin/{p_target_os}");
    println!("cargo:rustc-link-search=native={ffmpeg_bin_path}");

    // copy to target and deps folder
    copy_shared_libs(&ffmpeg_bin_path, &ffmpeg_dll_names);

    let libs = [
        "avcodec",
        "avdevice",
        "avfilter",
        "avformat",
        "avutil",
        "postproc",
        "swresample",
        "swscale",
    ];

    for lib in libs {
        println!("cargo:rustc-link-search=native={p_current_dir_path_str}/sys/third_party/ffmpeg/lib/{p_target_os}");
        println!("cargo:rustc-link-lib=static={lib}");
    }
}

#[cfg(feature = "media_openal")]
fn copy_openal(p_current_dir_path_str: &str, p_build_profile: &str) {
    let dll_names = ["OpenAL32.dll".to_owned()].to_vec();

    let bin_lib_path = format!(
        "{p_current_dir_path_str}/sys/build/{p_build_profile}/_deps/openal-build/{p_build_profile}",
    );
    println!("cargo:rustc-link-search=native={bin_lib_path}");

    // copy to target and deps folder
    copy_shared_libs(&bin_lib_path, &dll_names);
}

#[allow(clippy::too_many_lines)]
fn bindgens(p_current_dir_path_str: &str, p_build_profile: &str) {
    struct Binding<'a> {
        headers: Vec<String>,
        dst: &'a str,
        block_headers: &'a [&'a str],
        allow_funcs: &'a [&'a str],
    }
    let mut bindings = Vec::<Binding>::new();
    let mut clang_args = Vec::<String>::new();
    let pre_path = format!("{p_current_dir_path_str}/sys/build/{p_build_profile}");

    #[cfg(feature = "system_lz4")]
    {
        let include_path = format!("{pre_path}/_deps/lz4_static-src/lib");
        clang_args.push(format!("-I{include_path}"));
        bindings.push(Binding {
            headers: vec![format!("{include_path}/lz4.h")],
            dst: "src/system/ffi/lz4.rs",
            block_headers: &[],
            allow_funcs: &["LZ4_.*"],
        });
    }

    #[cfg(feature = "system_lzma")]
    bindings.push(Binding {
        headers: "sys/system/lzma.h",
        dst: "src/system/ffi/lzma.rs",
        block_headers: &[],
    });

    #[cfg(feature = "system_gamepad_virtual")]
    {
        let include_path = format!("{pre_path}/_deps/vigemclient-src/include");
        clang_args.push(format!("-I{include_path}"));
        bindings.push(Binding {
            headers: vec![format!("{include_path}/ViGEm/Client.h")],
            dst: "src/system/ffi/vigem_client.rs",
            block_headers: &[],
            allow_funcs: &["vigem_.*"],
        });
    }

    #[cfg(feature = "stream_rist")]
    bindings.push(Binding {
        headers: "sys/stream/rist.h",
        dst: "src/stream/ffi/rist.rs",
        block_headers: &[],
    });

    if cfg!(feature = "media_ffmpeg") {
        let include_path = format!("{p_current_dir_path_str}/sys/third_party/ffmpeg/include");
        clang_args.push(format!("-I{include_path}"));
        bindings.push(Binding {
            headers: vec![format!("{p_current_dir_path_str}/sys/media/ffmpeg.h")],
            dst: "src/media/ffi/ffmpeg.rs",
            block_headers: &[],
            allow_funcs: &[],
        });

        // bindgens.push(Binding {
        //     headers: &["sys/media/ffmpeg.h".to_owned()],
        //     dst: "src/media/ffi/ffmpeg.rs",
        //     block_headers: &[],
        //     allow_funcs: &[],
        // });
        // headers.push(Binding {
        //     src: "sys/media/test.h",
        //     dst: "src/media/ffi/test.rs",
        //     block_headers: &["sys/media/av_frame.h"],
        // });
    }

    // if cfg!(feature = "media_openal") {
    //     headers.push(Binding {
    //         headers: &["sys/media/openal.h".to_owned()],
    //         dst: "src/media/ffi/openal.rs",
    //         block_headers: &[],
    //     });
    // }

    for binding in bindings {
        // don't generate if already exists
        let out_path = Path::new(p_current_dir_path_str).join(binding.dst);
        if out_path.exists() {
            continue;
        }
        // The bindgen::Builder is the main entry point
        // to bindgen, and lets you build up options for
        // the resulting bindings.
        let mut builder = bindgen::Builder::default()
            .default_enum_style(bindgen::EnumVariation::Rust {
                non_exhaustive: false,
            })
            .raw_line(
                "#![allow(dead_code, non_camel_case_types, non_snake_case, trivial_numeric_casts)]",
            )
            .raw_line("#![allow(clippy::unreadable_literal, clippy::upper_case_acronyms)]")
            .layout_tests(false) // disable test
            .clang_args(["-x", "c++", "-std=c++20"])
            .clang_args(clang_args.clone())
            .generate_inline_functions(true)
            .header("sys/w_platform.h")
            // tell cargo to invalidate the built crate whenever any of the
            // included header files changed.
            .parse_callbacks(Box::new(bindgen::CargoCallbacks));

        // blocklist for Windows.h
        #[cfg(target_os = "windows")]
        {
            builder = builder
                .blocklist_type("_IMAGE_TLS_DIRECTORY64")
                .blocklist_type("IMAGE_TLS_DIRECTORY64")
                .blocklist_type("PIMAGE_TLS_DIRECTORY64")
                .blocklist_type("IMAGE_TLS_DIRECTORY")
                .blocklist_type("PIMAGE_TLS_DIRECTORY");
        }

        for header_file in binding.headers {
            println!("cargo:rerun-if-changed=sys/{header_file}");
            builder = builder.header(header_file);
        }

        // allowlist functions
        for fun in binding.allow_funcs {
            builder = builder.allowlist_function(fun);
        }

        // blocklist headers
        for block_header in binding.block_headers {
            builder = builder.blocklist_file(block_header);
        }

        // finish the builder and generate the bindings.
        builder
            .generate()
            // unwrap the Result and panic on failure.
            .unwrap_or_else(|e| {
                panic!(
                    "couldn't create bindings for header {:?} because {e:?}",
                    binding.dst
                );
            })
            .write_to_file(out_path)
            .unwrap_or_else(|e| {
                panic!(
                    "couldn't write bindings for {:?} because {e:?}",
                    binding.dst
                );
            });
    }
}
