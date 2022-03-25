use wolf_build::build::{BuildType, Dependency, FetchContentType, Session};
use wolf_macros::PB;

fn main() {
    //get the current path
    let current_dir = std::env::current_dir().expect("couldn't get current path directory");
    let output_rust_dir = current_dir.join("src/ffi");

    // build lz4
    let mut build_session = Session {
        name: "wolf_lz4",
        deps: &[Dependency {
            name: "lz4",
            fetch_content: FetchContentType::Vcpkg,
            headers: vec![
                PB!("lz4.h"),
                PB!("lz4frame_static.h"),
                PB!("lz4frame.h"),
                PB!("lz4hc.h"),
            ],
            rust_output_dir: &output_rust_dir,
        }],
        build_type: BuildType::NOP,
    };
    build_session.run();
}
