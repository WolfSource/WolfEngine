fn main() {
    let _build = cxx_build::bridge("src/lib.rs");
    println!("cargo:rerun-if-changed=src/lib.rs");

    // get the current path
    // let current_dir_path = std::env::current_dir().expect("could not get current directory");

    // let target_os =
    //     std::env::var("CARGO_CFG_TARGET_OS").expect("Build failed: could not get target OS");

    // if target_os == "macos" {
    //     std::env::set_var("MACOSX_DEPLOYMENT_TARGET", OSX_DEPLOYMENT_TARGET);
    // }

    // #[cfg(feature = "raft")]
    // build_protos(&current_dir_path, &target_os);
}

/// # Errors
///
/// Will return `Err` if `proto` does not exist or is invalid.
#[cfg(feature = "raft")]
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
#[cfg(feature = "raft")]
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
