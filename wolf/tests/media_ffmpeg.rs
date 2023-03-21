#[cfg(feature = "media_ffmpeg")]
fn encode(p_codec_id: &str) -> (wolf::media::ffmpeg::av_packet::AvPacket, (u32, u32)) {
    use image::GenericImageView;
    use wolf::media::{
        bindgen::ffmpeg::AVPixelFormat,
        ffmpeg::{
            av_config::AvConfig,
            av_frame::AvFrame,
            av_packet::AvPacket,
            ffmpeg_ctx::{AvCodecOpt, AvOptionValue, AvSetOpt, FFmpeg, Value},
        },
    };

    let current_dir = std::env::current_dir().unwrap();
    let mut path = current_dir.join("content/texture/rgb.png");
    if !path.exists() {
        path = current_dir.join("../content/texture/rgb.png");
    }

    // load image
    let img = image::open(path).unwrap();
    let img_size = img.dimensions();
    let pixels = img.as_rgba8().unwrap().to_vec();

    // create a source frame from img
    let rgba_config =
        AvConfig::new_video(AVPixelFormat::AV_PIX_FMT_RGBA, img_size.0, img_size.1, 1).unwrap();
    let rgba_frame = AvFrame::new(rgba_config, Vec::new(), pixels).unwrap();

    // create dst YUV frame
    let yuv_config =
        AvConfig::new_video(AVPixelFormat::AV_PIX_FMT_YUV420P, img_size.0, img_size.1, 1).unwrap();
    // convert src to yuv frame
    let yuv_frame = rgba_frame.convert_video(&yuv_config).unwrap();

    let codec_opt = AvCodecOpt {
        bitrate: 4_000_000,
        fps: 60,
        gop: 600,
        level: -1,
        max_b_frames: 2,
        refs: 3,
        thread_count: -1,
    };

    // for more info read https://trac.ffmpeg.org/wiki/Encode/AV1
    let opts = [
        AvSetOpt {
            name: "preset\0".to_owned(),
            value: AvOptionValue::Int(Value { i: 12 }),
        },
        AvSetOpt {
            name: "crf\0".to_owned(),
            value: AvOptionValue::Int(Value { i: 50 }),
        },
    ];

    let mut packet: AvPacket;
    let encoder =
        FFmpeg::new_encoder_from_codec_id_str(p_codec_id, &yuv_config, &codec_opt, &opts, None)
            .unwrap();
    loop {
        packet = encoder.encode(&yuv_frame, true).unwrap();
        let packet_size = packet.size();
        if packet_size > 0 {
            println!("encoded packet size is {packet_size}");
            break;
        }
    }

    (packet, img_size)
}

#[cfg(feature = "media_ffmpeg")]
fn decode(
    p_packet: &wolf::media::ffmpeg::av_packet::AvPacket,
    p_codec_id: &str,
    p_img_size: (u32, u32),
) {
    use wolf::media::{
        bindgen::ffmpeg::AVPixelFormat,
        ffmpeg::{
            av_config::AvConfig,
            av_frame::AvFrame,
            ffmpeg_ctx::{AvCodecOpt, FFmpeg},
        },
    };

    let mut current_dir = std::env::current_dir().unwrap();
    if current_dir.ends_with("wolf") {
        current_dir = current_dir.join("..");
    }
    let file_path = current_dir.join("content/texture/rgb_decoded.png");

    // create dst YUV frame
    let mut yuv_config = AvConfig::new_video(
        AVPixelFormat::AV_PIX_FMT_YUV420P,
        p_img_size.0,
        p_img_size.1,
        1,
    )
    .unwrap();
    let mut yuv_frame = AvFrame::new(yuv_config.clone(), Vec::new(), Vec::new()).unwrap();
    let codec_opt = AvCodecOpt {
        bitrate: 6_000_000,
        fps: 60,
        gop: -1,
        level: -1,
        max_b_frames: -1,
        refs: -1,
        thread_count: -1,
    };

    let decoder =
        FFmpeg::new_decoder_from_codec_id_str(p_codec_id, &mut yuv_config, &codec_opt, &[], None)
            .unwrap();
    decoder.decode(p_packet, &mut yuv_frame, false).unwrap();

    // create a source frame from img
    let rgb_config = AvConfig::new_video(
        AVPixelFormat::AV_PIX_FMT_RGBA,
        p_img_size.0,
        p_img_size.1,
        1,
    )
    .unwrap();
    // convert YUV to RGBA frame
    let rgba_frame = yuv_frame.convert_video(&rgb_config).unwrap();

    let pixels = rgba_frame.get_video_data_ptr(0).unwrap();
    image::save_buffer_with_format(
        file_path,
        pixels,
        p_img_size.0,
        p_img_size.1,
        image::ColorType::Rgba8,
        image::ImageFormat::Png,
    )
    .unwrap();
}

//cargo test test_encode_decode --release -- --exact --nocapture
#[cfg(feature = "media_ffmpeg")]
#[test]
fn test_encode_decode() {
    use std::time::Instant;

    let encoder_codec_id = "libsvtav1\0";
    let decoder_codec_id = "libdav1d\0";

    let mut t0 = Instant::now();
    let (packet, img_size) = encode(encoder_codec_id);
    let mut t1 = Instant::now();

    println!(
        "Codec ID:{:?} Encoded packet:{:?} Encode Time:{:?} ms",
        encoder_codec_id,
        packet.size(),
        (t1 - t0).as_millis()
    );

    t0 = Instant::now();
    decode(&packet, decoder_codec_id, img_size);
    t1 = Instant::now();
    println!(
        "Codec ID:{:?}    Decode Time:{:?} ms",
        decoder_codec_id,
        (t1 - t0).as_millis()
    );
}
