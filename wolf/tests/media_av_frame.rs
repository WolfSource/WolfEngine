#![allow(unused_crate_dependencies)]
#![allow(clippy::cast_possible_truncation)]

#[cfg(feature = "media_ffmpeg")]
#[test]
fn test_av_frame_convert() {
    use image::{EncodableLayout, GenericImageView};
    use wolf::media::av_frame::{AVFrame, AVPixelFormat};

    let current_dir = std::env::current_dir().unwrap();
    // load image
    let path = current_dir.join("sample_rgba.png");
    let img = image::open(path).unwrap();
    let img_size = img.dimensions();
    let pixels = img.as_rgba8().unwrap().as_bytes();

    // create a source frame from img
    let src_frame = AVFrame::new(AVPixelFormat::RGBA, img_size.0, img_size.1, 1, pixels).unwrap();

    // create dst frame
    let dst_frame_size =
        AVFrame::get_required_buffer_size(AVPixelFormat::RGBA, img_size.0, img_size.1, 1);
    let dst_frame_size_usize = dst_frame_size as usize;
    let mut dst_pixels = Vec::<u8>::with_capacity(dst_frame_size_usize);
    dst_pixels.fill(0u8);

    // create avframe
    let mut dst_frame = AVFrame::new(
        AVPixelFormat::BGRA,
        img_size.0,
        img_size.1,
        1,
        dst_pixels.as_bytes(),
    )
    .unwrap();

    src_frame.convert(&mut dst_frame).unwrap();
    let _ret = dst_frame.get_data(dst_pixels.as_mut_ptr());
    unsafe {
        dst_pixels.set_len(dst_frame_size_usize);
    }

    let out_path = current_dir.join("sample_bgra.png");
    image::save_buffer_with_format(
        out_path,
        &dst_pixels,
        img_size.0,
        img_size.1,
        image::ColorType::Rgba8,
        image::ImageFormat::Png,
    )
    .unwrap();
}
