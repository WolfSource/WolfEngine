#[macro_export]
// convert &str to String
macro_rules! STR {
    ($s:expr) => {
        ($s).to_owned();
    };
}

#[macro_export]
// convert &str to CString
macro_rules! CSTR {
    ($s:expr) => {
        std::ffi::CString::new($s);
    };
}

#[macro_export]
// convert &str to *const i8
macro_rules! STR_i8_PTR {
    ($a:expr, $b:expr) => {
        std::ffi::CString::new($a)
            .map(|cstr| {
                let bytes = cstr.as_bytes();
                (bytes.len(), bytes.as_ptr() as *const i8)
            })
            .map_err(|e| {
                anyhow::anyhow!("{:?}", e).context(format!("&str to *const i8 failed. {}", $b))
            });
    };
}

#[macro_export]
// convert &str to PathBuf
macro_rules! PB {
    ($a:expr) => {
        std::path::PathBuf::from($a);
    };
}
