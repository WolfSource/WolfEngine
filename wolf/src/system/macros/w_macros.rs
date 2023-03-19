#[cfg(not(target_arch = "wasm32"))]
#[macro_export]
macro_rules! w_log {
    ($fmt:expr) => {
        println!($fmt)
    };
    ($fmt:expr, $($args:tt)*) => {
        println!($fmt, $($args)*)
    };
}

#[cfg(not(target_arch = "wasm32"))]
#[macro_export]
macro_rules! w_new {
    ($ptr:expr, $obj:expr) => {
        match $obj {
            Ok(obj) => {
                *$ptr = Box::into_raw(Box::new(obj));
                WError::None
            }
            Err(e) => {
                *$ptr = std::ptr::null_mut();
                e
            }
        }
    };
}

#[cfg(not(target_arch = "wasm32"))]
#[macro_export]
macro_rules! w_free {
    ($ptr:expr) => {
        if $ptr.is_null() || (*$ptr).is_null() {
            return;
        }
        std::mem::drop(Box::from_raw(*$ptr));
        *$ptr = std::ptr::null_mut();
    };
}

#[cfg(target_arch = "wasm32")]
#[macro_export]
macro_rules! w_log {
    ($fmt:expr) => {
        web_sys::console::log_1(&($fmt).into())
    };
    ($fmt:expr, $($args:tt)*) => {
        let msg = format!($fmt, $($args)*);
        web_sys::console::log_1(&msg.into())
    };
}

// #[macro_export]
// macro_rules! w_defer {
//     ($($data: tt)*) => (
//         let _scope_call = ScopeCall {
//             cell: Some(|| -> () { { $($data)* } })
//         };
//     )
// }
