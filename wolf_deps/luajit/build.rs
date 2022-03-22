use wolf_build::build::{BuildType, Dependency, FetchContentType, Session};

#[allow(clippy::too_many_lines)]
fn main() {
    // get the current path
    let current_dir = std::env::current_dir().expect("couldn't get current path directory");
    let output_rust_dir = current_dir.join("src/ffi");
    let cmake_root_dir = current_dir.join("src/luajit-rocks");
    // get profile of build
    let profile = wolf_build::build::get_build_profile();
    // get path to the build folder of cmake
    let build_path = cmake_root_dir.join("build");
    // root of lib
    let lib = cmake_root_dir.join("build").join("luajit-2.1");

    // if build folder is exist don't launch cmake
    if !build_path.exists() {
        // root of include
        let include = cmake_root_dir.join("build").join(profile).join("include");

        // build luajit
        let mut build_session = Session {
            name: "wolf_luajit",
            deps: &[Dependency {
                name: "luajit",
                fetch_content: FetchContentType::NOP,
                headers: vec![
                    include.join("lauxlib.h"),
                    include.join("lua.h"),
                    include.join("luaconf.h"),
                    include.join("luajit.h"),
                    include.join("lualib.h"),
                ],
                rust_output_dir: &output_rust_dir,
            }],
            build_type: BuildType::CMake(
                cmake_root_dir,
                &["-DBUILD_SHARED_LIBS=FALSE", "-DBUILD_STATIC_LIBS=TRUE"],
            ),
        };
        build_session.run();
    }

    println!("cargo:rustc-link-search=native={}", lib.to_str().unwrap());
    println!("cargo:rustc-link-lib=static=luajit-static");
}
