#![allow(improper_ctypes)]

use std::os::raw::{c_char, c_int, c_void};

pub type w_rist_ctx = *mut c_void;

#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub enum rist_ctx_mode {
    RIST_SENDER_MODE = 0,
    RIST_RECEIVER_MODE,
}

#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub enum rist_profile {
    RIST_PROFILE_SIMPLE = 0,
    RIST_PROFILE_MAIN = 1,
    RIST_PROFILE_ADVANCED = 2,
}

#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub enum rist_log_level {
    RIST_LOG_DISABLE = -1,
    RIST_LOG_ERROR = 3,
    RIST_LOG_WARN = 4,
    RIST_LOG_NOTICE = 5,
    RIST_LOG_INFO = 6,
    RIST_LOG_DEBUG = 7,
    RIST_LOG_SIMULATE = 100,
}

extern "C" {
    fn w_rist_init(
        p_rist: *mut w_rist_ctx,
        p_url: *const c_char,
        p_mode: rist_ctx_mode,
        p_profile: rist_profile,
        p_simulate_loos: bool,
        p_loss_percentage: u16,
        p_log_level: rist_log_level,
        p_log_callback: extern "C" fn(*mut c_void, rist_log_level, *const c_char) -> c_int,
    ) -> c_int;
    fn w_rist_send(p_rist: w_rist_ctx, p_buffer: *const c_char, p_buffer_len: c_int) -> c_int;
    fn w_rist_fini(p_rist: *mut w_rist_ctx);
}

#[derive(Clone)]
pub struct rist {
    pub ctx: w_rist_ctx,
    pub url: String,
    pub mode: rist_ctx_mode,
    pub profile: rist_profile,
    pub simulate_loos: bool,
    pub loss_percentage: u16,
    pub log_level: rist_log_level,
}

impl Drop for rist {
    fn drop(&mut self) {
        //drop rist context
        unsafe { w_rist_fini(&mut self.ctx) };
    }
}

impl rist {
    pub fn new(
        p_url: &str,
        p_mode: rist_ctx_mode,
        p_profile: rist_profile,
        p_simulate_loos: bool,
        p_loss_percentage: u16,
        p_log_level: rist_log_level,
        p_log_callback: extern "C" fn(*mut c_void, rist_log_level, *const c_char) -> c_int,
    ) -> anyhow::Result<Self> {
        let mut obj = Self {
            ctx: std::ptr::null_mut(),
            url: p_url.to_owned(),
            mode: p_mode,
            profile: p_profile,
            simulate_loos: p_simulate_loos,
            loss_percentage: p_loss_percentage,
            log_level: p_log_level,
        };

        // create sender/receiver context
        let ret = unsafe {
            w_rist_init(
                &mut obj.ctx,
                obj.url.as_ptr() as *const c_char,
                obj.mode,
                obj.profile,
                obj.simulate_loos,
                obj.loss_percentage,
                obj.log_level,
                p_log_callback,
            )
        };
        match ret {
            0 => Ok(obj),
            _ => {
                anyhow::bail!("could not create rist {:?}", obj.mode)
            }
        }
    }

    pub fn send(&self, p_buffer: &[u8], p_buffer_len: u32) -> anyhow::Result<()> {
        let len = p_buffer_len as i32;
        let ret = unsafe { w_rist_send(self.ctx, p_buffer.as_ptr() as *const c_char, len) };
        match ret {
            0 => Ok(()),
            _ => {
                anyhow::bail!("could not send the buffer")
            }
        }
    }
}
