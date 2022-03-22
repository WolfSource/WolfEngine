use wolf_build::{
    build::{BuildSession, BuildType, DepConfig},
    proto,
};

fn main() {
    //get the current path
    let current_dir_path_buffer = std::env::current_dir().expect("couldn't get current directory");
    let current_dir = current_dir_path_buffer
        .to_str()
        .expect("couldn't get current directory");
    //get the root of VCPKG
    let vcpkg_root =
        std::env::var("VCPKG_ROOT").expect("couldn't get environment variable VCPKG_ROOT");

    // compile protos
    proto::compile_protos_from_dirs(&["./proto"], "couldn't read the protos directory");

    // run wolf_build
    let include_1 = format!("{}/installed/x64-osx/include", vcpkg_root);
    let include_2 = format!("{}/../include", current_dir);
    let includes = [include_1.as_str(), include_2.as_str()];

    let deps = [DepConfig {
        build_type: BuildType::Vcpkg,
        name: "lz4",
        rust_src_paths: &["src/compression/lz4.rs"],
        cpp_src_paths: &["src/compression/cxx/lz4/"],
    }];
    let build_session = BuildSession {
        name: "wolf_system_cxx",
        includes: Some(&includes),
        deps: Some(&deps),
    };
    build_session.run();
}
