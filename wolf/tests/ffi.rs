#[cfg(feature = "ffi")]
#[test]
fn generate_ffi_c() {
    use interoptopus::Interop;
    use interoptopus_backend_c::{Config, Generator};

    let mut path = std::env::current_dir().unwrap();
    if path.ends_with("wolf") {
        path = path.join("../ffi/wolf.h");
    }

    Generator::new(
        Config {
            ifndef: "_WOLF_H_".to_string(),
            ..Config::default()
        },
        wolf::ffi_inventory(),
    )
    .write_file(path)
    .unwrap();
}

#[cfg(feature = "ffi")]
#[test]
fn generate_ffi_python() {
    use interoptopus::Interop;
    use interoptopus_backend_cpython::{Config, Generator};

    let mut path = std::env::current_dir().unwrap();
    if path.ends_with("wolf") {
        path = path.join("../ffi/wolf.py");
    }

    let library = wolf::ffi_inventory();
    Generator::new(Config::default(), library)
        .write_file(path)
        .unwrap();
}
