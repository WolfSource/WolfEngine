//cargo test test_audio_devices --release -- --exact --nocapture
#[cfg(feature = "media_audio")]
#[test]
fn test_audio_devices() {
    use wolf::media::audio::Audio;

    let all_audio_hosts = Audio::get_all_hosts();
    println!("All audio hosts: {all_audio_hosts:?}");

    let all_available_hosts = Audio::get_available_hosts();
    println!("All available audio hosts: {all_available_hosts:?}");

    if all_available_hosts.is_empty() {
        return;
    }

    if let Some(host) = all_available_hosts.get(0) {
        let io_devices_res = Audio::get_default_devices(*host);
        match io_devices_res {
            Ok(io_devices) => {
                if let Some(in_device) = io_devices.0 {
                    let _d0 = Audio::get_device_default_input_config(&in_device);
                    let _s0 = Audio::get_device_supported_input_config(&in_device);
                }
                if let Some(out_device) = io_devices.1 {
                    let _d1 = Audio::get_device_default_input_config(&out_device);
                    let _s1 = Audio::get_device_supported_input_config(&out_device);
                }
            }
            Err(_e) => {}
        }
    }
}

//cargo test test_play_beep --release -- --exact --nocapture
#[cfg(feature = "media_audio")]
#[test]
fn test_play_beep() {
    use cpal::{
        traits::{DeviceTrait, HostTrait},
        SampleFormat,
    };
    use wolf::{error::WError, media::audio::Audio};

    let host = Audio::get_default_host();
    let device_res = host.default_output_device();
    if let Some(device) = device_res {
        println!("Output device: {:?}", device.name());
        let config_res = device.default_output_config();
        match config_res {
            Ok(config) => {
                let post_wait = instant::Duration::from_millis(1000);
                let _res = match config.sample_format() {
                    SampleFormat::I8 => Audio::play::<i8>(&device, &config.into(), None, post_wait),
                    SampleFormat::I16 => {
                        Audio::play::<i16>(&device, &config.into(), None, post_wait)
                    }
                    SampleFormat::I32 => {
                        Audio::play::<i32>(&device, &config.into(), None, post_wait)
                    }
                    SampleFormat::I64 => {
                        Audio::play::<i64>(&device, &config.into(), None, post_wait)
                    }
                    SampleFormat::U8 => Audio::play::<u8>(&device, &config.into(), None, post_wait),
                    SampleFormat::U16 => {
                        Audio::play::<u16>(&device, &config.into(), None, post_wait)
                    }
                    SampleFormat::U32 => {
                        Audio::play::<u32>(&device, &config.into(), None, post_wait)
                    }
                    SampleFormat::U64 => {
                        Audio::play::<u64>(&device, &config.into(), None, post_wait)
                    }
                    SampleFormat::F32 => {
                        Audio::play::<f32>(&device, &config.into(), None, post_wait)
                    }
                    SampleFormat::F64 => {
                        Audio::play::<f64>(&device, &config.into(), None, post_wait)
                    }
                    sample_format => {
                        println!("Unsupported sample format '{sample_format}'");
                        Err(WError::MediaAudioPlayStreamFailed)
                    }
                };
            }
            Err(e) => {
                println!("Could not get default output config because {:?}", e);
            }
        }
    }
}
