#![allow(unused_crate_dependencies)]

#[cfg(feature = "system_lz4")]
use 
{   std::{ffi::CStr, os::raw::c_char, ptr, time::Instant},
    wolf::system::compression::lz4::{self, size_t, w_buf, w_buf_t}
};

// Executes and verifies compression and decompression of a buffer
#[cfg(feature = "system_lz4")]
fn action_executor(
    name: &str,
    acceleration: i32,
    doer: Box<dyn Fn() -> i32>,
    buffer_pointer: *mut w_buf_t,
    trace_pointer: *mut w_buf_t,
) {
    let started_at = Instant::now();
    let doer_result = doer();
    let duration = Instant::now() - started_at;

    let buffer = unsafe { *buffer_pointer };

    let trace = unsafe { CStr::from_ptr((*trace_pointer).data as *mut c_char) };
    assert_eq!(
        doer_result, 0,
        "Error occurred while {} (acceleration ratio = `{}`) the buffer: `{:?}`. Trace info: `{:?}`!",
        name, acceleration, buffer, trace
    );

    println!(
        "LZ4 {} (acceleration={}) just done in `{}` second(s). Memory location was `{:?}` with the size: `{}`",
        name,
        acceleration,
        duration.as_secs_f64(),
        buffer.data,
        buffer.len,
    );
}

#[cfg(feature = "system_lz4")]
fn test_lz4_compression(mock_compression_data: &str, acceleration: i32, fast_mode: bool) {
    // Compression
    let mut trace_data = [0u8; 256];
    let mut trace_buffer = w_buf_t {
        data: trace_data.as_mut_ptr(),
        len: 256,
    };
    let trace_buffer_ptr = &mut trace_buffer as w_buf;

    let mut source_buffer = w_buf_t {
        data: mock_compression_data.as_ptr() as *mut u8,
        len: mock_compression_data.len() as size_t,
    };
    let source_buffer_ptr = &mut source_buffer as w_buf;

    let mut compressed_buffer = w_buf_t {
        data: ptr::null_mut(),
        len: 0,
    };
    let compressed_buffer_ptr = &mut compressed_buffer as w_buf;
    let compressing = Box::new(move || unsafe {
        lz4::w_lz4_compress(
            source_buffer_ptr,
            compressed_buffer_ptr,
            fast_mode,
            acceleration,
            trace_buffer_ptr,
        )
    });

    action_executor(
        "compressing",
        acceleration,
        compressing,
        compressed_buffer_ptr,
        trace_buffer_ptr,
    );

    // Decompression
    let mut decompressed_buffer = w_buf_t {
        data: ptr::null_mut(),
        len: 0,
    };
    let decompressed_buffer_ptr = &mut decompressed_buffer as w_buf;

    let decompressing = Box::new(move || unsafe {
        lz4::w_lz4_decompress(
            compressed_buffer_ptr,
            decompressed_buffer_ptr,
            trace_buffer_ptr,
        )
    });
    action_executor(
        "decompressing",
        acceleration,
        decompressing,
        decompressed_buffer_ptr,
        trace_buffer_ptr,
    );

    let decompressed_data = unsafe { CStr::from_ptr((*decompressed_buffer_ptr).data as *const i8) };
    let decompressed_data = decompressed_data.to_str().unwrap();

    assert_eq!(
        mock_compression_data, decompressed_data,
        "The content of the compressed and decompressed buffer aren't the same!"
    );

    if unsafe { lz4::w_lz4_free_buf(compressed_buffer_ptr) } != 0 {
        println!("Couldn't free compressed buffer!");
    }
    if unsafe { lz4::w_lz4_free_buf(decompressed_buffer_ptr) } != 0 {
        println!("Couldn't free decompressed buffer!");
    }
}

#[cfg(feature = "system_lz4")]
#[test]
fn test_lz4() {
    let mock_compression_data = "HELLO WOLF\r\nHELLO WOLF!*&%!HELLO WOLF!07*&%!\r\nThe quick brown fox jumps over the lazy dog!";
    println!(
        "The content of the data is `{}` with the size `{}`",
        mock_compression_data,
        mock_compression_data.len(),
    );

    for fast_mode in 0..2 {
        let fast_mode = fast_mode == 0;

        if fast_mode {
            println!("\nTesting with fast mode enabled");
        } else {
            println!("\nTesting with fast mode disabled");
        }

        for i in 0..=10 {
            test_lz4_compression(mock_compression_data, i, fast_mode);
        }
    }
}
