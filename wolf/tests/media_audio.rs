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
                    let _s1 = Audio::get_device_default_input_config(&in_device);
                    let _i1 = Audio::get_device_supported_input_config(&in_device);
                }
                if let Some(out_device) = io_devices.1 {
                    let _s1 = Audio::get_device_default_input_config(&out_device);
                    let _i1 = Audio::get_device_supported_input_config(&out_device);
                }
            }
            Err(_e) => {}
        }
    }
}
