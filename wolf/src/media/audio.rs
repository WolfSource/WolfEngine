use crate::error::WError;
use cpal::{
    traits::{DeviceTrait, HostTrait},
    Device, HostId, SupportedInputConfigs, SupportedStreamConfig,
};

#[derive(Clone, Copy, Debug)]
pub enum SampleRate {
    _44100 = 44_100,
    _48000 = 48_000,
}

#[derive(Clone, Copy, Debug)]
pub enum Format {
    /// 8-bit mono buffer format
    Mono8,
    /// 16-bit mono buffer format
    Mono16,
    /// 8-bit stereo buffer format
    Stereo8,
    /// 16-bit stereo buffer format
    Stereo16,
}

#[derive(Clone, Debug)]
pub struct Audio {}

impl Drop for Audio {
    fn drop(&mut self) {}
}

impl Audio {
    pub fn get_all_hosts() -> Vec<HostId> {
        cpal::ALL_HOSTS.to_vec()
    }

    pub fn get_available_hosts() -> Vec<HostId> {
        cpal::available_hosts()
    }

    pub fn get_default_devices(
        p_host_id: HostId,
    ) -> Result<(Option<Device>, Option<Device>), WError> {
        let host = cpal::host_from_id(p_host_id).map_err(|_e| WError::MediaAudioHostWasNotFound)?;
        Ok((host.default_input_device(), host.default_output_device()))
    }

    pub fn get_device_default_input_config(
        p_device: &Device,
    ) -> Result<SupportedStreamConfig, WError> {
        p_device
            .default_input_config()
            .map_err(|_e| WError::MediaAudioStreamConfigNotFound)
    }

    pub fn get_device_supported_input_config(
        p_device: &Device,
    ) -> Result<SupportedInputConfigs, WError> {
        p_device
            .supported_input_configs()
            .map_err(|_e| WError::MediaAudioSupportedInputConfigNotFound)
    }

    // pub fn find_input_host(p_output_name: &str) -> Vec<HostId> {
    //     cpal::available_hosts()
    // }

    //     /// # Errors
    //     ///
    //     /// TODO: add error description
    //     #[allow(clippy::cast_possible_wrap)]
    //     #[allow(clippy::cast_possible_truncation)]
    //     pub fn new(
    //         p_format: Format,
    //         p_sample_rate: SampleRate,
    //         p_refresh: u32,
    //         p_number_of_channels: std::num::NonZeroU32,
    //     ) -> Result<Self> {
    //         let mut obj = Self {
    //             ctx: std::ptr::null_mut(),
    //             format: p_format,
    //             sample_rate: p_sample_rate,
    //             refresh: p_refresh,
    //         };

    //         // create a buffer for error
    //         let mut buf = [1i8; W_MAX_PATH as usize];
    //         let buf_ptr = buf.as_mut_ptr();

    //         let channels = p_number_of_channels.get() as i32;
    //         unsafe {
    //             let ret = w_openal_init(
    //                 &mut obj.ctx,
    //                 obj.format as i32,
    //                 obj.sample_rate as i32,
    //                 obj.refresh as i32,
    //                 channels,
    //                 buf_ptr,
    //             );
    //             if ret == 0 {
    //                 Ok(obj)
    //             } else {
    //                 let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
    //                 let str = c_err_str.to_str().unwrap_or_default();
    //                 bail!(
    //                     "could not create openal object because {}",
    //                     String::from(str)
    //                 )
    //             }
    //         }
    //     }

    //     /// # Safety
    //     ///
    //     /// TODO: add safety description
    //     /// # Errors
    //     ///
    //     /// TODO: add error description
    //     pub fn open(&self) -> Result<()> {
    //         // create a buffer for error
    //         let mut buf = [1i8; W_MAX_PATH as usize];
    //         let buf_ptr = buf.as_mut_ptr();

    //         unsafe {
    //             let ret = w_openal_open(self.ctx, buf_ptr);
    //             if ret == 0 {
    //                 Ok(())
    //             } else {
    //                 let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
    //                 let str = c_err_str.to_str().unwrap_or_default();
    //                 bail!(
    //                     "could not create openal object because {}",
    //                     String::from(str)
    //                 )
    //             }
    //         }
    //     }

    //     /// # Errors
    //     ///
    //     /// TODO: add error description
    //     pub fn update_from_stack_i16(&self, p_audio_frame_buffer: &[i16]) -> Result<()> {
    //         let mut buf = [1i8; W_MAX_PATH as usize];
    //         let buf_ptr = buf.as_mut_ptr();

    //         unsafe {
    //             let ret = w_openal_update_i16(
    //                 self.ctx,
    //                 p_audio_frame_buffer.as_ptr(),
    //                 p_audio_frame_buffer.len(),
    //                 buf_ptr,
    //             );
    //             if ret == 0 {
    //                 Ok(())
    //             } else {
    //                 let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
    //                 let str = c_err_str.to_str().unwrap_or_default();
    //                 bail!(
    //                     "could not update openal object because {}",
    //                     String::from(str)
    //                 )
    //             }
    //         }
    //     }

    //     /// # Errors
    //     ///
    //     /// TODO: add error description
    //     pub fn update_from_heap_i16(&self, p_audio_frame_buffer: &Vec<i16>) -> Result<()> {
    //         let mut buf = [1i8; W_MAX_PATH as usize];
    //         let buf_ptr = buf.as_mut_ptr();

    //         unsafe {
    //             let ret = w_openal_update_i16(
    //                 self.ctx,
    //                 p_audio_frame_buffer.as_ptr(),
    //                 p_audio_frame_buffer.len(),
    //                 buf_ptr,
    //             );
    //             if ret == 0 {
    //                 Ok(())
    //             } else {
    //                 let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
    //                 let str = c_err_str.to_str().unwrap_or_default();
    //                 bail!(
    //                     "could not update openal object because {}",
    //                     String::from(str)
    //                 )
    //             }
    //         }
    //     }

    //     /// reset openal
    //     pub fn reset(&self) {
    //         unsafe {
    //             w_openal_reset(self.ctx);
    //         }
    //     }

    //     /// close openal
    //     pub fn close(&self, p_force_stop: bool) {
    //         unsafe {
    //             w_openal_close(self.ctx, p_force_stop);
    //         }
    //     }

    //     /// # Errors
    //     ///
    //     /// TODO: add error description
    //     pub fn list_all_devices() -> Result<(String, String)> {
    //         // create two buffers for input & output devices
    //         let mut input = [0i8; W_MAX_PATH as usize];
    //         let mut output = [0i8; W_MAX_PATH as usize];
    //         let input_ptr = input.as_mut_ptr();
    //         let output_ptr = output.as_mut_ptr();

    //         unsafe {
    //             w_openal_list_all_devices(output_ptr, input_ptr);

    //             let in_cstr = std::ffi::CStr::from_ptr(input_ptr);
    //             let out_cstr = std::ffi::CStr::from_ptr(output_ptr);

    //             let in_devices = in_cstr.to_str().unwrap_or_default();
    //             let out_devices = out_cstr.to_str().unwrap_or_default();

    //             Ok((String::from(out_devices), String::from(in_devices)))
    //         }
    //     }
}
