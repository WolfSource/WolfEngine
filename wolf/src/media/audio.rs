use crate::error::WError;
use cpal::{
    traits::{DeviceTrait, HostTrait, StreamTrait},
    Device, FromSample, Host, HostId, Sample, SizedSample, StreamConfig, SupportedInputConfigs,
    SupportedStreamConfig,
};
use instant::Duration;

#[derive(Clone, Debug)]
pub struct Audio {}

impl Drop for Audio {
    fn drop(&mut self) {}
}

impl Audio {
    #[must_use]
    pub fn get_all_hosts() -> Vec<HostId> {
        cpal::ALL_HOSTS.to_vec()
    }

    #[must_use]
    pub fn get_available_hosts() -> Vec<HostId> {
        cpal::available_hosts()
    }

    #[must_use]
    pub fn get_default_host() -> Host {
        cpal::default_host()
    }

    /// get default audio devices
    /// # Errors
    ///
    /// returns an `WError`
    pub fn get_default_devices(
        p_host_id: HostId,
    ) -> Result<(Option<Device>, Option<Device>), WError> {
        let host = cpal::host_from_id(p_host_id).map_err(|_e| WError::MediaAudioHostWasNotFound)?;
        Ok((host.default_input_device(), host.default_output_device()))
    }

    /// get default input config from audio device
    /// # Errors
    ///
    /// returns an `WError`
    pub fn get_device_default_input_config(
        p_device: &Device,
    ) -> Result<SupportedStreamConfig, WError> {
        p_device
            .default_input_config()
            .map_err(|_e| WError::MediaAudioStreamConfigNotFound)
    }

    /// get default supported config from audio device
    /// # Errors
    ///
    /// returns an `WError`
    pub fn get_device_supported_input_config(
        p_device: &Device,
    ) -> Result<SupportedInputConfigs, WError> {
        p_device
            .supported_input_configs()
            .map_err(|_e| WError::MediaAudioSupportedInputConfigNotFound)
    }

    fn write_data<T>(p_output: &mut [T], p_channels: usize, p_next_sample: &mut dyn FnMut() -> f32)
    where
        T: Sample + FromSample<f32>,
    {
        for frame in p_output.chunks_mut(p_channels) {
            let value: T = T::from_sample(p_next_sample());
            for sample in frame.iter_mut() {
                *sample = value;
            }
        }
    }

    /// play stream of audio
    /// # Errors
    ///
    /// returns an `WError`
    pub fn play<T>(
        p_device: &Device,
        p_config: &StreamConfig,
        p_timeout: Option<Duration>,
        p_post_wait: Duration,
    ) -> Result<(), WError>
    where
        T: SizedSample + FromSample<f32>,
    {
        #[allow(clippy::cast_precision_loss)]
        let sample_rate = p_config.sample_rate.0 as f32;
        let channels = p_config.channels as usize;

        // produce a sinusoid of maximum amplitude.
        let mut sample_clock = 0f32;
        let mut next_value = move || {
            sample_clock = (sample_clock + 1.0) % sample_rate;
            (sample_clock * 440.0 * 2.0 * std::f32::consts::PI / sample_rate).sin()
        };

        let err_fn = |err| eprintln!("an error occurred on stream: {err}");

        let stream = p_device
            .build_output_stream(
                p_config,
                move |data: &mut [T], _cb: &cpal::OutputCallbackInfo| {
                    Self::write_data(data, channels, &mut next_value);
                },
                err_fn,
                p_timeout,
            )
            .map_err(|_e| WError::MediaAudioBuildStreamFailed)?;
        stream
            .play()
            .map_err(|_e| WError::MediaAudioBuildStreamFailed)?;

        std::thread::sleep(p_post_wait);
        Ok(())
    }
}
