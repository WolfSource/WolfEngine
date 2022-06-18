#![allow(unused_crate_dependencies)]

use wolf::media::ffi::ffmpeg::w_ffmpeg_opt_t;

#[test]
fn test_encode() {
    let av_codec_id_av1 = 226;
    const LIMIT: usize = 256;
    let mut error = ['0' as i8; LIMIT];
    let mut size_out = 0;
    let mut ref_out_null = 0 as *mut u8;
    let mut data_out = ref_out_null as *mut *mut u8;
    let mut data_in = 0 as *mut u8;

    // read raw rgb data from a sample file

    let options = &mut w_ffmpeg_opt_t {
        fps: 60,
        width: 1920,
        height: 1080,
        bitrate: 400000,
        codec_id: av_codec_id_av1,
        ctx: std::ptr::null_mut(),
    } as *mut w_ffmpeg_opt_t;

    // let _ret = wolf::ffmpeg_encode(
    //     options,
    //     data_in,
    //     data_out,
    //     &mut size_out,
    //     error.as_mut_ptr(),
    // );
}
