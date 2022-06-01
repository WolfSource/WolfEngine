#![allow(improper_ctypes)]

use std::os::raw::{c_char, c_int, c_void};

pub type w_rist_ctx = *mut c_void;

#[derive(Clone)]
pub enum RistMode {
    SENDER,
    RECEIVER,
}

impl std::fmt::Debug for RistMode {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::SENDER => write!(f, "RistMode_SENDER"),
            Self::RECEIVER => write!(f, "RistMode_RECEIVER"),
        }
    }
}

#[derive(Clone)]
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

impl std::fmt::Debug for rist_log_level {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::RIST_LOG_DISABLE => write!(f, "RIST_LOG_DISABLE"),
            Self::RIST_LOG_ERROR => write!(f, "RIST_LOG_ERROR"),
            Self::RIST_LOG_WARN => write!(f, "RIST_LOG_WARN"),
            Self::RIST_LOG_NOTICE => write!(f, "RIST_LOG_NOTICE"),
            Self::RIST_LOG_INFO => write!(f, "RIST_LOG_INFO"),
            Self::RIST_LOG_DEBUG => write!(f, "RIST_LOG_DEBUG"),
            Self::RIST_LOG_SIMULATE => write!(f, "RIST_LOG_SIMULATE"),
        }
    }
}

extern "C" {
    fn w_rist_receiver_create(
        p_rist: *mut w_rist_ctx,
        p_log_callback: extern "C" fn(*mut c_void, rist_log_level, *const c_char) -> c_int,
    ) -> std::os::raw::c_int;
    fn w_rist_destroy(p_rist: *mut w_rist_ctx);
}

#[derive(Clone)]
pub struct rist {
    pub ctx: w_rist_ctx,
    pub mode: RistMode,
}

impl Drop for rist {
    fn drop(&mut self) {
        //drop rist context
        unsafe { w_rist_destroy(&mut self.ctx) };
    }
}

impl rist {
    pub fn new(
        p_mode: RistMode,
        p_log_callback: extern "C" fn(*mut c_void, rist_log_level, *const c_char) -> c_int,
    ) -> anyhow::Result<Self> {
        let mut obj = Self {
            ctx: std::ptr::null_mut(),
            mode: p_mode,
        };

        // create sender/receiver context
        let ret = match obj.mode {
            RistMode::SENDER => 0,
            RistMode::RECEIVER => unsafe { w_rist_receiver_create(&mut obj.ctx, p_log_callback) },
        };
        match ret {
            0 => Ok(obj),
            _ => {
                anyhow::bail!("could not create rist {:?}", obj.mode)
            }
        }
    }
}
