use anyhow::{bail, Result};
use gstreamer_rtsp::*;
use gstreamer_rtsp_server::{
    prelude::RTSPServerExtManual,
    traits::{RTSPMediaFactoryExt, RTSPMountPointsExt, RTSPServerExt},
    RTSPMediaFactory, RTSPServer, RTSPTransportMode,
};
use std::sync::Once;

static ONCE_INIT: Once = Once::new();
static ONCE_FINI: Once = Once::new();

struct rtsp_gst {}

impl rtsp_gst {
    pub fn init() -> () {
        ONCE_INIT.call_once(|| {
            let _ = gst::init().and_then(|_| Ok(())).or_else(|e| {
                bail!(
                    "could not initialize gstreamer because {:?}. Trace: rtsp_gst::init",
                    e,
                )
            });
        });
    }

    pub fn fini() {
        ONCE_FINI.call_once(|| {
            unsafe { gst::deinit() };
        });
    }
}

#[tokio::main]
#[test]
async fn tests() -> () {
    println!("hello");
}
