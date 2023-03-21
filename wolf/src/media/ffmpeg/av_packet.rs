use crate::{
    error::WError,
    media::bindgen::ffmpeg::{av_packet_alloc, av_packet_free, av_packet_unref, AVPacket},
};

#[derive(Clone)]
pub struct AvPacket {
    pub pkt: *mut AVPacket,
    pub own_data: Vec<u8>,
}

impl Drop for AvPacket {
    fn drop(&mut self) {
        if !self.pkt.is_null() {
            unsafe { av_packet_free(&mut self.pkt) };
        }
        #[cfg(debug_assertions)]
        println!("AvPacket dropped");
    }
}

impl AvPacket {
    /// # Errors
    ///
    /// returns an `WError`
    pub fn new() -> Result<Self, WError> {
        Self::new_from_data(Vec::new())
    }

    /// # Errors
    ///
    /// returns an `WError`
    pub fn new_from_data(mut p_data: Vec<u8>) -> Result<Self, WError> {
        unsafe {
            let pkt = av_packet_alloc();
            if pkt.is_null() {
                return Err(WError::OutOfMemory);
            }
            let size = i32::try_from(p_data.len()).map_err(|_e| WError::SizeCastError)?;
            if !p_data.is_empty() {
                (*pkt).data = p_data.as_mut_ptr();
                (*pkt).size = size;
            }
            Ok(Self {
                pkt,
                own_data: p_data,
            })
        }
    }

    /// unref `AvPacket`
    pub fn unref(&self) {
        unsafe { av_packet_unref(self.pkt) };
    }

    /// get packet size of `AvPacket`
    #[must_use]
    #[allow(clippy::missing_const_for_fn)]
    pub fn size(&self) -> i32 {
        unsafe { (*self.pkt).size }
    }
}
