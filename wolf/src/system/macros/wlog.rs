#[cfg(not(target_arch = "wasm32"))]
#[macro_export]
macro_rules! wlog {
    ($fmt:expr) => {
        println!($fmt)
    };
    ($fmt:expr, $($args:tt)*) => {
        println!($fmt, $($args)*)
    };
}

#[cfg(target_arch = "wasm32")]
#[macro_export]
macro_rules! wlog {
    ($fmt:expr) => {
        web_sys::console::log_1(&($fmt).into())
    };
    ($fmt:expr, $($args:tt)*) => {
        let msg = format!($fmt, $($args)*);
        web_sys::console::log_1(&msg.into())
    };
}
