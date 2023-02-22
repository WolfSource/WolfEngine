use wasm_bindgen::closure::{Closure, WasmClosureFnOnce};
use wasm_bindgen::{prelude::wasm_bindgen, JsValue};

#[wasm_bindgen(module = "/src/stream/webrtc/janus_rs.js")]
extern "C" {
    fn w_janus_init(p_debug_mode: String, p_on_init_callback: JsValue) -> i32;
    fn w_janus_joinroom(p_room_id: f64, p_username: String) -> i32;
    fn w_janus_sharescreen(
        p_username: String,
        p_room_desc: String,
        p_bitrate: i32,
        p_on_screenshared: JsValue,
    ) -> i32;
}

#[derive(Debug, thiserror::Error)]
pub enum JanusError {
    #[error("Janus failed on initializing")]
    InitFailed,
}

#[derive(Debug)]
pub struct Janus {}

impl Janus {
    pub fn init<F>(p_debug_mode: String, p_on_init: F) -> Result<(), JanusError>
    where
        F: 'static + WasmClosureFnOnce<bool, ()>,
    {
        let ret = w_janus_init(p_debug_mode, Closure::once_into_js(p_on_init));
        if ret != 0 {
            return Err(JanusError::InitFailed);
        }
        Ok(())
    }

    pub fn join_room(p_room_id: f64, p_username: String) {
        let _ret = w_janus_joinroom(p_room_id, p_username);
    }

    pub fn share_screen<F>(
        p_username: String,
        p_room_desc: String,
        p_bitrate: i32,
        p_on_screenshare: F,
    ) where
        F: 'static + WasmClosureFnOnce<f64, ()>,
    {
        let _ret = w_janus_sharescreen(
            p_username,
            p_room_desc,
            p_bitrate,
            Closure::once_into_js(p_on_screenshare),
        );
    }
}
