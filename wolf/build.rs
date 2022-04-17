use std::{io, path::Path, process::Command};

fn main() {
    // get the current path
    let current_dir_path = std::env::current_dir().expect("could not get current directory");
    let current_dir_path_str = current_dir_path
        .to_str()
        .expect("could not convert current dir path to &str");

    //get current target os
    let target_os =
        std::env::var("CARGO_CFG_TARGET_OS").expect("Build failed: could not get target OS");
    // compile protos
    let mut build_server_proto = true;
    let build_client_proto = true;
    // we don't want proto server for android and ios clients
    if target_os == "android" || target_os == "ios" {
        build_server_proto = false;
    }

    let wolf_lib_ext = if target_os == "windows" {
        "wolf_cc.lib"
    } else {
        "libwolf_cc.a"
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

    // execute cmake
    build_cmake(&current_dir_path, wolf_lib_ext);

    // link to wolf_cc library
    println!("cargo:rustc-link-search=native=/usr/lib/");
    println!(
        "cargo:rustc-link-search=native={}/cc/build/Debug/",
        current_dir_path_str
    );
    println!("cargo:rustc-link-lib=static=wolf_cc");
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
pub fn build_cmake(p_current_path: &Path, p_wolf_cc_file_name: &str) {
    // get current build profile
    let profile_str = std::env::var("PROFILE").expect("could not get PROFILE");
    // get opt_level
    let opt_level_str = std::env::var("OPT_LEVEL").expect("could not get OPT_LEVEL profile");
    // set CMake build config
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

    // get parent path
    let cmake_current_path = p_current_path.join("cc/");
    let cmake_current_path_str = cmake_current_path
        .to_str()
        .expect("could not convert cmake current path to &str");

    // get cmake build path
    let install_folder = format!("build/{}", build_profile);
    let cmake_build_path = cmake_current_path.join(install_folder);
    let cmake_build_path_str = cmake_build_path
        .to_str()
        .expect("could not convert cmake build path to &str");

    // return if wolf_cc library was found
    let wolf_cc_path = cmake_build_path.join(p_wolf_cc_file_name);
    if std::path::Path::new(&wolf_cc_path).exists() {
        return;
    }
    // configure
    let mut out = Command::new("cmake")
        .current_dir(&cmake_current_path)
        .args([
            ".",
            "--no-warn-unused-cli",
            "-Wdev",
            "--debug-output",
            "-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE",
            format!("-B{}", cmake_build_path_str).as_str(),
            format!("-S{}", cmake_current_path_str).as_str(),
            format!("-DCMAKE_BUILD_TYPE:STRING={}", build_profile).as_str(),
        ])
        .output()
        .expect("could not configure cmake of wolf/cc");

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
            build_profile,
            "--parallel",
            "8",
        ])
        .output()
        .expect("could not build cmake of wolf/cc");

    assert!(
        out.status.success(),
        "CMake Build failed for {} because: {:?}",
        cmake_current_path_str,
        std::str::from_utf8(&out.stderr)
    );

    // generate rust codes from c headers
    generate_bindgens(p_current_path);
}

fn generate_bindgens(p_current_path: &Path) {
    // list of headers
    let headers = ["wolf"];

    let out_rust_path = p_current_path.join("src/ffi/");
    let path = p_current_path.join("cc");
    for header in headers {
        let header_name = format!("{}.h", header);
        let rust_name = format!("{}.rs", header);

        // create a path buffer to header
        let pb = path.join(header_name);
        let h = pb.to_str().unwrap();
        let binding = bindgen::Builder::default()
            .header(h)
            .generate()
            .expect("unable to generate rust bindings");

        let out = out_rust_path.join(rust_name);
        binding
            .write_to_file(out)
            .unwrap_or_else(|e| panic!("couldn't write bindings for {} because {}", h, e));
    }
}
