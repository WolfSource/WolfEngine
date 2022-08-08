#[cfg(not(target_arch = "wasm32"))]
#[macro_export]
macro_rules! w_fn_once {
    ($($args:tt)*) => {
        core::ops::function::FnOnce($($args)*)
    };
}

#[cfg(target_arch = "wasm32")]
#[macro_export]
macro_rules! w_fn_once {
    ($($args:tt)*) => {
        serde_closure::FnOnce!($($args)*)
    };
}
