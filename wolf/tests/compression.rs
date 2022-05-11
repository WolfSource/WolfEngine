#![allow(unused_crate_dependencies)]

use wolf::ffi::lz4::size_t;

#[test]
fn test_lz4() {
    use wolf::ffi::lz4::{buf, buf_t, compress, decompress, free_buf};

    let content = "HELLO WOLF\r\nHELLO WOLF!*&%!HELLO WOLF!07*&%!\r\nThe quick brown fox jumps over the lazy dog!";
    println!("original memory is {} with size {}", content, content.len(),);

    let mut source_buffer = buf {
        data: content.as_ptr() as *mut u8,
        len: content.len() as size_t,
    };
    let source_buffer_ptr = &mut source_buffer as buf_t;

    let mut compressed_buffer = buf {
        data: std::ptr::null_mut(),
        len: 0,
    };
    let compressed_buffer_ptr = &mut compressed_buffer as buf_t;

    let mut trace_data = [0u8; 256];
    let mut trace_buffer = buf {
        data: trace_data.as_mut_ptr(),
        len: 256,
    };
    let trace_buffer_ptr = &mut trace_buffer as buf_t;

    let fast_mode = 1;
    let acceleration = 1;

    // compress
    let mut now = std::time::Instant::now();
    let mut ret = unsafe {
        compress(
            source_buffer_ptr,
            compressed_buffer_ptr,
            fast_mode,
            acceleration,
            trace_buffer_ptr,
        )
    };
    let mut duration = std::time::Instant::now() - now;
    if ret == 0 {
        println!(
            "lz4 compressed just done in {} sec(s). memory is {:?} with size: {}",
            duration.as_secs_f64(),
            compressed_buffer.data,
            compressed_buffer.len,
        );
    } else {
        let str = unsafe { std::ffi::CStr::from_ptr(trace_buffer.data as *mut i8) };
        println!("error just happened while compressing buffer {:?}", str);
    }

    // decompress
    let mut decompressed_buffer = buf {
        data: std::ptr::null_mut(),
        len: 0,
    };
    let decompressed_buffer_ptr = &mut decompressed_buffer as buf_t;

    now = std::time::Instant::now();
    ret = unsafe {
        decompress(
            compressed_buffer_ptr,
            decompressed_buffer_ptr,
            trace_buffer_ptr,
        )
    };
    duration = std::time::Instant::now() - now;
    if ret == 0 {
        let content = unsafe { std::ffi::CStr::from_ptr(decompressed_buffer.data as *mut i8) };
        println!(
            "lz4 decompressed just done in {} sec(s). memory is {:?} with size: {}",
            duration.as_secs_f64(),
            content,
            decompressed_buffer.len,
        );
    } else {
        let str = unsafe { std::ffi::CStr::from_ptr(trace_buffer.data as *mut i8) };
        println!("error just happened while decompressing buffer {:?}", str);
    }

    // release all memories
    unsafe {
        if free_buf(compressed_buffer_ptr) != 0 {
            println!("could not free compressed buffer");
        }
        if free_buf(decompressed_buffer_ptr) != 0 {
            println!("could not free decompressed buffer");
        }
    };
}