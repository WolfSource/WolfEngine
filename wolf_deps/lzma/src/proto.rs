use std::{fs, io};

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
