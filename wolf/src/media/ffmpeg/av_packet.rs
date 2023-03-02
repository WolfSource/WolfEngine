use super::av_error::AvError;
use crate::media::binding::ffmpeg::{av_packet_alloc, av_packet_free, AVPacket};

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
    /// returns an AvError if the object could not be created
    pub fn new() -> Result<Self, AvError> {
        let pkt = unsafe { av_packet_alloc() };
        if pkt.is_null() {
            return Err(AvError::OutOfMemory);
        }
        Ok(Self { pkt })
    }
}
