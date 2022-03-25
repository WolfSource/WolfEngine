use wolf_build::proto;

fn main() {
    // compile protos
    proto::compile_protos_from_dirs(&["./proto"], "couldn't read the protos directory");

    // // run wolf_build
    // let include_1 = format!("{}/installed/x64-osx/include", vcpkg_root);
    // let include_2 = format!("{}/../include", current_dir);
    // let includes = [include_1.as_str(), include_2.as_str()];

    // let deps = [DepConfig {
    //     build_type: BuildType::Vcpkg,
    //     name: "lz4",
    //     rust_src_paths: &["src/compression/lz4.rs"],
    //     cpp_src_paths: &["src/compression/cxx/lz4/"],
    // }];
    // let build_session = BuildSession {
    //     name: "wolf_system_cxx",
    //     includes: Some(&includes),
    //     deps: Some(&deps),
    // };
    // build_session.run();
}
