use crate::{
    error::WError,
    media::bindgen::ffmpeg::{avformat_alloc_context, avformat_free_context, AVFormatContext},
};

#[derive(Debug, Clone)]
pub struct AvFormatContext {
    // av format context
    pub ctx: *mut AVFormatContext,
}

impl Drop for AvFormatContext {
    fn drop(&mut self) {
        if self.ctx.is_null() {
            return;
        }
        unsafe { avformat_free_context(self.ctx) };
        #[cfg(debug_assertions)]
        println!("AvFormat dropped");
    }
}

impl AvFormatContext {
    /// # Errors
    ///
    /// returns an `WError`
    pub fn new() -> Result<Self, WError> {
        let ctx = unsafe { avformat_alloc_context() };
        if ctx.is_null() {
            return Err(WError::MediaAvFormatContextAllocFailed);
        }
        Ok(Self { ctx })
    }
}
