#[macro_export]
// convert &str to String
macro_rules! S {
    ($a:expr) => {
        ($a).to_owned();
    };
}

#[macro_export]
// convert &str to PathBuf
macro_rules! PB {
    ($a:expr) => {
        std::path::PathBuf::from($a);
    };
}
