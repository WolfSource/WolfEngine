#![allow(unused_crate_dependencies)]

#[tokio::test]
async fn test_lz4() {
    use std::os::raw::*;

    let content = "HELLO WOLF\r\nHELLO WOLF!*&%!HELLO WOLF!07*&%!\r\nThe quick brown fox jumps over the lazy dog!";
    println!("original memory is {} with size {}", content, content.len(),);

    let source_buffer = content.as_bytes();
    let source_buffer_len = source_buffer.len();

    let compressed_buffer: *mut c_char = std::ptr::null_mut();
    let mut compressed_buffer_len = 0;

    let fast_mode = 1;
    let acceleration = 1;

    let mut trace_buffer = [0 as c_char; 256];

    let now = std::time::Instant::now();
    let ret = unsafe {
        wolf::ffi::lz4::compress(
            source_buffer.as_ptr() as *const i8,
            source_buffer_len as i32,
            compressed_buffer,
            &mut compressed_buffer_len,
            fast_mode,
            acceleration,
            trace_buffer.as_mut_ptr(),
        )
    };
    let duration = std::time::Instant::now() - now;
    if ret == 0 {
        println!(
            "lz4 compressed done in {} sec(s). Memory is {:?} with size: {}",
            duration.as_secs_f64(),
            compressed_buffer,
            compressed_buffer_len,
        );
    } else {
        let str = unsafe { std::ffi::CString::from_raw(trace_buffer.as_mut_ptr()) };
        println!("error happened while compressing buffer {:?}", str);
    }

    //     now = std::time::Instant::now();
    //     let decompressed = lz4::ffi::decompress(compressed.as_slice(), &mut trace);
    //     duration = std::time::Instant::now() - now;
    //     println!(
    //         "lz4 decompressed done in {} sec. Memory is {:?}. trace info: {:?}",
    //         duration.as_secs_f64(),
    //         std::str::from_utf8(decompressed.as_slice()),
    //         trace
    //     );
}
