// #![allow(unused_crate_dependencies)]
#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
#[global_allocator]
static GLOBAL: mimalloc::MiMalloc = mimalloc::MiMalloc;
pub mod media;
//#[cfg(feature = "render")]
//pub mod render;
pub mod stream;
pub mod system;

#[cfg(feature = "ffi")]
#[interoptopus::ffi_type]
#[repr(C)]
pub enum FFIError {
    /// All went fine.
    Ok = 0,
}

#[cfg(feature = "ffi")]
#[interoptopus::ffi_type]
#[repr(C)]
pub struct w_string {
    pub data: *mut u8,
    pub len: u64,
}

/// Returns the version of this API.
#[cfg(feature = "ffi")]
#[no_mangle]
#[interoptopus::ffi_function]
pub extern "C" fn wolf_version() -> u32 {
    0x00_03_00_03
}

/// Returns the version of this API.
#[cfg(feature = "ffi")]
#[no_mangle]
#[interoptopus::ffi_function]
pub extern "C" fn wolf_string(p_arg: *mut w_string) -> FFIError {
    unsafe {
        std::ptr::write(
            p_arg,
            w_string {
                data: "Hello World!".as_ptr() as *mut u8,
                len: "Hello World!".len() as u64,
            },
        )
    };
    FFIError::Ok
}

#[cfg(feature = "ffi")]
pub fn ffi_inventory() -> interoptopus::Inventory {
    {
        use interoptopus::function;
        interoptopus::InventoryBuilder::new()
            .register(function!(wolf_version))
            .register(function!(wolf_string))
            .inventory()
    }
}
