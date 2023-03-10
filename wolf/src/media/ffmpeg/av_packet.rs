use crate::{
    error::WError,
    media::ffi::ffmpeg::{av_packet_alloc, av_packet_free, AVPacket},
};

#[derive(Clone)]
pub struct AvPacket {
    pkt: *mut AVPacket,
}

impl Drop for AvPacket {
    fn drop(&mut self) {
        if !self.pkt.is_null() {
            unsafe { av_packet_free(&mut self.pkt) };
        }
    }
}

impl AvPacket {
    /// # Errors
    ///
    /// returns an `WError`
    pub fn new() -> Result<Self, WError> {
        let pkt = unsafe { av_packet_alloc() };
        if pkt.is_null() {
            return Err(WError::OutOfMemory);
        }
        Ok(Self { pkt })
    }
}
