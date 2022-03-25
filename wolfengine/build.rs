use std::{fs, io};

fn main() {
    // compile protos
    compile_protos_from_dirs(&["./proto"], "couldn't read the protos directory");

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

/// # Panics
///
/// Will panic if a directory entry be Err.
pub fn compile_protos_from_dirs(p_proto_dirs: &[&str], p_custom_err_message: &str) {
    for iter in p_proto_dirs {
        let dir_entries = fs::read_dir(*iter).expect(p_custom_err_message);
        // iterate over all protos inside this path
        for entry in dir_entries {
            let entry_pathbuf = entry.unwrap().path();
            // compile only proto
            if let Some(extension) = entry_pathbuf.extension() {
                if extension == "proto" {
                    tonic_build::compile_protos(entry_pathbuf).unwrap();
                }
            }
        }
    }
}
