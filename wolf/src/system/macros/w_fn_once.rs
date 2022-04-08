#[cfg(not(feature = "wasm"))]
#[macro_export]
macro_rules! w_fn_once {
    ($($args:tt)*) => {
        core::ops::function::FnOnce($($args)*)
    };
}

#[cfg(feature = "wasm")]
#[macro_export]
macro_rules! w_fn_once {
    ($($args:tt)*) => {
        serde_closure::FnOnce!($($args)*)
    };
}
