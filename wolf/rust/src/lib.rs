#[cxx::bridge]
mod ffi {
    #[namespace = "wolf_rs"]
    extern "Rust" {
        fn fun() -> i32;
    }
}

#[inline(always)]
fn fun() -> i32 {
    return 0;
}
