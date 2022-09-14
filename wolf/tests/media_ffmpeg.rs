#![allow(unused_crate_dependencies)]
#![allow(clippy::cast_possible_truncation)]
//use wolf::media::av_frame::{AVFrame, AVPixelFormat};
// use wolf::media::ffmpeg::FFmpeg;
// use image::EncodableLayout;
// use std::fs::File;
// use wolf::media::ffi::ffmpeg::w_ffmpeg_opt_t;
// use wolf::media::ffmpeg::ffmpeg::{self, *};

static IMAGE_DIR: &str = "G:/SourceCodes/WolfEngine_rs/WolfEngine/wolf";

#[cfg(feature = "media_ffmpeg")]
#[test]
fn test_encode() {
    use image::{EncodableLayout, GenericImageView};
    use wolf::media::av_frame::{AVFrame, AVPixelFormat};

    // load image
    let path = std::path::Path::new(IMAGE_DIR).join("sample.png");
    let img = image::open(path).unwrap();
    let img_size = img.dimensions();
    let pixels = img.as_rgba8().unwrap().as_bytes();

    // create a source frame from img
    let _src_frame = AVFrame::new(AVPixelFormat::RGBA, img_size.0, img_size.1, 1, pixels).unwrap();

    //let _ffmpeg = FFmpeg::new();

    // // initalize variables
    // let av_codec_id_av1 = 226;
    // const LIMIT: usize = 255;
    // let width: i64 = 1024;
    // let height: i64 = 1024;
    // let mut error = ['0' as u8; LIMIT];

    // let mut encoded_buffer = CBuffer {
    //     ptr: std::ptr::null_mut(),
    //     len: 0,
    // };

    // let encode_options = &mut w_ffmpeg_opt_t {
    //     fps: 60,
    //     width: width,
    //     height: height,
    //     bitrate: 400000,
    //     codec_id: av_codec_id_av1,
    //     ctx: std::ptr::null_mut(),
    //     type_: w_ffmpeg_action_W_ENCODE,
    // } as *mut w_ffmpeg_opt_t;

    // // open jpeg file sample
    // let input_image = Path::new("Logo.jpg");
    // let img = image::open(input_image).unwrap();
    // let tmp = img.as_bytes();

    // // initialize ffmpeg
    // let mut res = ffmpeg_init(encode_options, &error);

    // match res {
    //     Ok(()) => {
    //         w_log!("FFMPEG is initialized successfully.");
    //     }
    //     Err(e) => {
    //         w_log!("Initializing failed: {:?}", error);
    //     }
    // };

    // // encode the sample image
    // res = ffmpeg_encode(encode_options, tmp, &mut encoded_buffer, &error);

    // match res {
    //     Ok(()) => {
    //         w_log!(
    //             "Image is encoded successfully. The encoded file size is: {}",
    //             encoded_buffer.len
    //         );
    //     }
    //     Err(e) => {
    //         w_log!("Encoding failed: {:?}", error);
    //     }
    // };

    // // open file to write encoding buffer into
    // let encoded_file_path = Path::new("encoded_buffer.svt1");
    // let encoded_file_display = encoded_file_path.display();
    // let mut encoded_file = match File::create(encoded_file_path) {
    //     Err(why) => panic!("couldn't create {}: {}", encoded_file_display, why),
    //     Ok(file) => file,
    // };

    // let data_in = unsafe {
    //     let buf = std::slice::from_raw_parts(encoded_buffer.ptr, encoded_buffer.len as usize);
    //     buf
    // };

    // // write encoded buffer into file
    // res = match encoded_file.write_all(data_in) {
    //     Err(why) => panic!("couldn't write to file {}: {}", encoded_file_display, why),
    //     Ok(()) => Ok(()),
    // };
}

#[cfg(feature = "media_ffmpeg")]
#[test]
const fn test_decode() {
    // use std::{
    //     io::{Read, Write},
    //     path::Path,
    // };
    // use wolf::{media::ffi::ffmpeg::w_ffmpeg_action_W_DECODE, w_log};

    // // initialize variables
    // let av_codec_id_av1 = 226;
    // const LIMIT: usize = 255;
    // let width: u32 = 1024;
    // let height: u32 = 1024;
    // let mut error = ['0' as u8; LIMIT];

    // let mut decoded_buffer = CBuffer {
    //     ptr: std::ptr::null_mut(),
    //     len: 0,
    // };

    // let decode_options = &mut w_ffmpeg_opt_t {
    //     fps: 60,
    //     width: width as i64,
    //     height: height as i64,
    //     bitrate: 400000,
    //     codec_id: av_codec_id_av1,
    //     ctx: std::ptr::null_mut(),
    //     type_: w_ffmpeg_action_W_DECODE,
    // } as *mut w_ffmpeg_opt_t;

    // // open encoding file sample
    // let encoded_file_path = Path::new("encoded_buffer.svt1");
    // let encoded_file_display = encoded_file_path.display();
    // let mut data_in = Vec::new();
    // let mut file = match File::open(encoded_file_path) {
    //     Err(why) => panic!("couldn't open {}: {}", encoded_file_display, why),
    //     Ok(file) => file,
    // };

    // // read the encoding file sample
    // let _file_open_res = match file.read_to_end(&mut data_in) {
    //     Err(why) => panic!("couldn't read {}: {}", encoded_file_display, why),
    //     Ok(file) => file,
    // };

    // // initialize ffmpeg
    // let mut res = ffmpeg_init(decode_options, &error);

    // match res {
    //     Ok(()) => {
    //         w_log!("FFMPEG is initialized successfully.");
    //     }
    //     Err(e) => {
    //         w_log!("Initializing failed: {:?}", error);
    //     }
    // };

    // // decode the sample buffer
    // let mut res = ffmpeg_decode(
    //     decode_options,
    //     data_in.as_bytes(),
    //     data_in.len().try_into().unwrap(),
    //     &mut decoded_buffer,
    //     &error,
    // );

    // match res {
    //     Ok(()) => {
    //         w_log!(
    //             "Image is decoded successfully. The decoded file size is: {}",
    //             decoded_buffer.len
    //         );
    //     }
    //     Err(e) => {
    //         w_log!("Decoding failed: {:?}", error);
    //     }
    // };

    // // open file to write decoding buffer into
    // let decoded_file_path = Path::new("decoded_buffer.svt1");
    // let decoded_file_display = decoded_file_path.display();
    // let mut decoded_file = match File::create(decoded_file_path) {
    //     Err(why) => panic!("couldn't create {}: {}", decoded_file_display, why),
    //     Ok(file) => file,
    // };

    // let data_out = unsafe {
    //     let buf = std::slice::from_raw_parts(decoded_buffer.ptr, decoded_buffer.len as usize);
    //     buf
    // };

    // // write decoded buffer into file
    // res = match decoded_file.write_all(data_out) {
    //     Err(why) => panic!("couldn't write to file {}: {}", decoded_file_display, why),
    //     Ok(()) => Ok(()),
    // };

    // // save decoded buffer into a image
    // image::save_buffer(
    //     "decoded_image.jpg",
    //     data_out,
    //     width,
    //     height,
    //     image::ColorType::Rgb8,
    // )
    // .unwrap();
}
