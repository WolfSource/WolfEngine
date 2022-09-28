#[cfg(feature = "media_ffmpeg")]
fn encode(
    p_packet: &mut wolf::media::av_packet::AVPacket,
    p_codec_id: wolf::media::ffmpeg::AVCodecID,
) -> (u32, u32) {
    use image::{EncodableLayout, GenericImageView};
    use wolf::media::{
        av_frame::{AVFrame, AVPixelFormat},
        ffi::ffmpeg::AVCodeOptions,
        ffmpeg::{FFmpeg, FFmpegMode},
    };

    let current_dir = std::env::current_dir().unwrap();
    // load image
    let path = current_dir.join("src.png");
    let img = image::open(path).unwrap();
    let img_size = img.dimensions();
    let pixels = img.as_rgba8().unwrap().as_bytes();

    // create a source frame from img
    let rgba_frame = AVFrame::new(AVPixelFormat::RGBA, img_size.0, img_size.1, 1, pixels).unwrap();

    // create dst YUV frame
    let mut yuv_frame =
        AVFrame::new(AVPixelFormat::YUV420P, img_size.0, img_size.1, 1, &[]).unwrap();

    // convert src to dst frame
    rgba_frame.convert(&mut yuv_frame).unwrap();

    let codec_opt = AVCodeOptions {
        bitrate: 6000000,
        fps: 60,
        gop: -1,
        level: -1,
        max_b_frames: -1,
        refs: -1,
        thread_count: -1,
    };

    let encoder = FFmpeg::new(&yuv_frame, FFmpegMode::ENCODER, p_codec_id, codec_opt, &[]).unwrap();

    loop {
        let packet_size = encoder.encode(&yuv_frame, p_packet).unwrap();
        if packet_size > 0 {
            println!("encoded packet size is {}", packet_size);
            break;
        }
    }

    img_size
}

#[cfg(feature = "media_ffmpeg")]
fn decode(
    p_packet: &wolf::media::av_packet::AVPacket,
    p_codec_id: wolf::media::ffmpeg::AVCodecID,
    p_img_size: (u32, u32),
) {
    use wolf::media::{
        av_frame::{AVFrame, AVPixelFormat},
        ffi::ffmpeg::AVCodeOptions,
        ffmpeg::{FFmpeg, FFmpegMode},
    };
    let current_dir = std::env::current_dir().unwrap();

    // create dst YUV frame
    let mut yuv_frame =
        AVFrame::new(AVPixelFormat::YUV420P, p_img_size.0, p_img_size.1, 1, &[]).unwrap();
    let codec_opt = AVCodeOptions {
        bitrate: 6000000,
        fps: 60,
        gop: -1,
        level: -1,
        max_b_frames: -1,
        refs: -1,
        thread_count: -1,
    };
    let decoder = FFmpeg::new(&yuv_frame, FFmpegMode::DECODER, p_codec_id, codec_opt, &[]).unwrap();

    decoder.decode(p_packet, &mut yuv_frame).unwrap();

    // create a source frame from img
    let mut rgba_frame =
        AVFrame::new(AVPixelFormat::RGBA, p_img_size.0, p_img_size.1, 1, &[]).unwrap();
    // convert YUV to RGBA frame
    yuv_frame.convert(&mut rgba_frame).unwrap();

    let pixels = rgba_frame.get_data(0).unwrap();
    let out_path = current_dir.join("src_decoded.png");
    image::save_buffer_with_format(
        out_path,
        pixels,
        p_img_size.0,
        p_img_size.1,
        image::ColorType::Rgba8,
        image::ImageFormat::Png,
    )
    .unwrap();
}

#[cfg(feature = "media_ffmpeg")]
#[test]
fn test_encode_decode() {
    use std::time::Instant;

    use wolf::media::{av_packet::AVPacket, ffmpeg::AVCodecID};
    println!("wolf_sys version is : {:?}", wolf::sys_init());

    let codec_id = AVCodecID::AV1;

    let mut packet = AVPacket::new().unwrap();
    let mut t0 = Instant::now();
    let img_size = encode(&mut packet, codec_id.clone());
    let mut t1 = Instant::now();

    let (_, packet_size) = packet.get_data();
    println!(
        "CodeID:{:?}    Encoded packet:{:?} Encode Time:{:?} ms",
        codec_id,
        packet_size,
        (t1 - t0).as_millis()
    );

    t0 = Instant::now();
    decode(&packet, codec_id.clone(), img_size);
    t1 = Instant::now();
    println!(
        "CodeID:{:?}    Decode Time:{:?} ms",
        codec_id,
        (t1 - t0).as_millis()
    );
}
