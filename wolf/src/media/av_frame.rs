// #[derive(Clone)]
// pub struct AVFrame {
//     audio_config: av_audio_config,
//     video_config: av_video_config,
//     frame: *mut AVFrame,
// }

// impl Drop for AVFrame {
//     fn drop(&mut self) {
//         if !self.ctx.is_null() {
//             unsafe { w_av_frame_fini(&mut self.ctx) };
//         }
//     }
// }

// impl AVFrame {
//     /// # Errors
//     ///
//     /// TODO: add error description
//     pub fn new(
//         p_pixel_format: AVPixelFormat,
//         p_width: u32,
//         p_height: u32,
//         p_alignment: u32,
//         p_data: &[u8],
//     ) -> Result<Self> {
//         #[allow(clippy::pedantic)]
//         // get data buffer size
//         let buffer_size =
//             Self::get_required_buffer_size(p_pixel_format, p_width, p_height, p_alignment);

//         // create self object
//         let mut obj = Self {
//             ctx: std::ptr::null_mut(),
//             pixel_format: p_pixel_format,
//             width: p_width,
//             height: p_height,
//             align: p_alignment,
//             data: vec![],
//         };

//         // if there is data, make a copy from it
//         obj.data.resize(buffer_size, 0u8);
//         if !p_data.is_empty() {
//             obj.data.copy_from_slice(p_data);
//         }

//         // create a buffer for error
//         let mut err = [1i8; W_MAX_PATH as usize];
//         let err_ptr = err.as_mut_ptr();

//         unsafe {
//             let ret = w_av_frame_init(
//                 &mut obj.ctx,
//                 obj.pixel_format as u32,
//                 obj.width,
//                 obj.height,
//                 obj.align,
//                 obj.data.as_ptr(),
//                 err_ptr,
//             );

//             if ret == 0 {
//                 Ok(obj)
//             } else {
//                 let c_err_str = std::ffi::CStr::from_ptr(err_ptr);
//                 let str = c_err_str.to_str().unwrap_or_default();
//                 bail!(
//                     "could not create av_frame object because {}. trace: {:?}",
//                     String::from(str),
//                     std::backtrace::Backtrace::force_capture()
//                 )
//             }
//         }
//     }

//     #[must_use]
//     pub fn get_required_buffer_size(
//         p_pixel_format: AVPixelFormat,
//         p_width: u32,
//         p_height: u32,
//         p_alignment: u32,
//     ) -> usize {
//         unsafe {
//             w_av_get_required_buffer_size(p_pixel_format as u32, p_width, p_height, p_alignment)
//         }
//     }

//     /// # Errors
//     ///
//     /// TODO: add error description
//     pub fn set_data(&mut self, p_data: &[u8]) -> Result<()> {
//         // create a buffer for error
//         let mut err = [1i8; W_MAX_PATH as usize];
//         let err_ptr = err.as_mut_ptr();

//         if !p_data.is_empty() {
//             self.data.copy_from_slice(p_data);
//         }

//         unsafe {
//             let ret = w_av_set_data(self.ctx, self.data.as_ptr(), self.align, err_ptr);
//             if ret == 0 {
//                 Ok(())
//             } else {
//                 let c_err_str = std::ffi::CStr::from_ptr(err_ptr);
//                 let str = c_err_str.to_str().unwrap_or_default();
//                 bail!(
//                     "could not set data to av_frame because {}. trace: {:?}",
//                     String::from(str),
//                     std::backtrace::Backtrace::force_capture()
//                 )
//             }
//         }
//     }

//     /// # Errors
//     ///
//     /// TODO: add error description
//     pub fn get_data(&mut self, p_index: usize) -> Result<&[u8]> {
//         // create a buffer for error
//         let mut err = [1i8; W_MAX_PATH as usize];
//         let err_ptr = err.as_mut_ptr();

//         unsafe {
//             let ret = w_av_get_data(self.ctx, p_index, self.data.as_mut_ptr(), err_ptr);
//             if ret == 0 {
//                 Ok(&self.data)
//             } else {
//                 let c_err_str = std::ffi::CStr::from_ptr(err_ptr);
//                 let str = c_err_str.to_str().unwrap_or_default();
//                 bail!(
//                     "could not set data to av_frame because {}. trace: {:?}",
//                     String::from(str),
//                     std::backtrace::Backtrace::force_capture()
//                 )
//             }
//         }
//     }

//     /// # Errors
//     ///
//     /// TODO: add error description
//     pub fn get_data_linesize(&mut self, p_index: usize) -> Result<i32> {
//         // create a buffer for error
//         let mut err = [1i8; W_MAX_PATH as usize];
//         let err_ptr = err.as_mut_ptr();

//         unsafe {
//             let size = w_av_get_data_linesize(self.ctx, p_index, err_ptr);
//             if size < 0 {
//                 let c_err_str = std::ffi::CStr::from_ptr(err_ptr);
//                 let str = c_err_str.to_str().unwrap_or_default();
//                 bail!(
//                     "could not set data to av_frame because {}. trace: {:?}",
//                     String::from(str),
//                     std::backtrace::Backtrace::force_capture()
//                 )
//             }
//             Ok(size)
//         }
//     }

//     /// # Errors
//     ///
//     /// TODO: add error description
//     pub fn convert(&self, p_dst: &mut Self) -> Result<()> {
//         // create a buffer for error
//         let mut err = [1i8; W_MAX_PATH as usize];
//         let err_ptr = err.as_mut_ptr();

//         unsafe {
//             let ret = w_av_frame_convert(self.ctx, &mut p_dst.ctx, err_ptr);
//             if ret == 0 {
//                 Ok(())
//             } else {
//                 let c_err_str = std::ffi::CStr::from_ptr(err_ptr);
//                 let str = c_err_str.to_str().unwrap_or_default();
//                 bail!(
//                     "could not create av_frame object because {}. trace: {:?}",
//                     String::from(str),
//                     std::backtrace::Backtrace::force_capture()
//                 )
//             }
//         }
//     }
// }
