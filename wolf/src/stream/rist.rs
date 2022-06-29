#![allow(improper_ctypes)]

use crate::stream::ffi::rist::{
    w_receiver_data_callback, w_rist_auth_connect_callback, w_rist_auth_disconnect_callback,
    w_rist_connect, w_rist_connection_status_callback, w_rist_ctx, w_rist_data_block, w_rist_fini,
    w_rist_free_data_block, w_rist_get_data_block, w_rist_get_data_block_len, w_rist_init,
    w_rist_init_data_block, w_rist_log_callback, w_rist_out_of_band_callback, w_rist_read,
    w_rist_set_auth_handler, w_rist_set_conn_status_callback, w_rist_set_data_block,
    w_rist_set_out_of_band_callback, w_rist_set_receiver_data_callback, w_rist_set_stats_callback,
    w_rist_stats_callback, w_rist_write,
};
use crate::system::ffi::version::size_t;
use anyhow::{bail, Result};
use std::os::raw::c_void;

#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub enum rist_connection_status {
    RIST_CONNECTION_ESTABLISHED = 0,
    RIST_CONNECTION_TIMED_OUT,
    RIST_CLIENT_CONNECTED,
    RIST_CLIENT_TIMED_OUT,
}

#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub enum rist_ctx_mode {
    RIST_SENDER_MODE = 0,
    RIST_RECEIVER_MODE = 1,
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
impl rist_log_level {
    #[must_use]
    pub const fn from_i32(value: i32) -> Self {
        match value {
            3 => Self::RIST_LOG_ERROR,
            4 => Self::RIST_LOG_WARN,
            5 => Self::RIST_LOG_NOTICE,
            6 => Self::RIST_LOG_INFO,
            7 => Self::RIST_LOG_DEBUG,
            100 => Self::RIST_LOG_SIMULATE,
            _ => Self::RIST_LOG_DISABLE,
        }
    }
}

#[derive(Clone)]
pub struct rist_data_block {
    pub block: w_rist_data_block,
}

impl Default for rist_data_block {
    fn default() -> Self {
        Self {
            block: std::ptr::null_mut(),
        }
    }
}

impl Drop for rist_data_block {
    fn drop(&mut self) {
        if !self.block.is_null() {
            unsafe { w_rist_free_data_block(&mut self.block) };
        }
    }
}

impl rist_data_block {
    /// # Errors
    ///
    /// TODO: add error description
    pub fn new() -> Result<Self> {
        let mut rist_block = Self {
            block: std::ptr::null_mut(),
        };
        let ret = unsafe { w_rist_init_data_block(&mut rist_block.block) };
        if ret == 0 {
            return Ok(rist_block);
        }
        bail!("could not allocate memory for rist_data_block")
    }

    /// # Errors
    ///
    /// TODO: add error description
    pub fn from(p_data_block: w_rist_data_block) -> Result<Self> {
        let mut rist_block = Self {
            block: p_data_block,
        };
        let ret = unsafe { w_rist_init_data_block(&mut rist_block.block) };
        if ret == 0 {
            return Ok(rist_block);
        }
        bail!("could not allocate memory for rist_data_block")
    }

    /// # Errors
    ///
    /// TODO: add error description
    pub fn read(&mut self) -> Result<&[u8]> {
        const TRACE: &str = "rist::read";
        unsafe {
            let ptr = w_rist_get_data_block(self.block);
            let len = w_rist_get_data_block_len(self.block);
            let len_usize = usize::try_from(len).unwrap_or(0);
            if len_usize > 0 {
                return Ok(std::slice::from_raw_parts(ptr as *mut u8, len_usize));
            }
            bail!("invalid len size. trace info {}", TRACE)
        }
    }

    pub fn set(&mut self, p_data: &[u8]) {
        unsafe {
            w_rist_set_data_block(
                self.block,
                p_data.as_ptr().cast::<std::ffi::c_void>(),
                p_data.len() as size_t,
            );
        };
    }
}

#[derive(Clone)]
pub struct rist {
    pub ctx: w_rist_ctx,
    pub url: String,
    pub mode: rist_ctx_mode,
    pub profile: rist_profile,
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
    /// # Errors
    ///
    /// TODO: add error description
    pub fn new(
        p_mode: rist_ctx_mode,
        p_profile: rist_profile,
        p_loss_percentage: u16,
        p_log_level: rist_log_level,
        p_log_callback: w_rist_log_callback,
    ) -> Result<Self> {
        let mut obj = Self {
            ctx: std::ptr::null_mut(),
            url: "".to_owned(),
            mode: p_mode,
            profile: p_profile,
            loss_percentage: p_loss_percentage,
            log_level: p_log_level,
        };

        // create sender/receiver context
        let ret = unsafe {
            w_rist_init(
                &mut obj.ctx,
                obj.mode as i32,
                obj.profile as i32,
                obj.loss_percentage,
                obj.log_level as i32,
                p_log_callback,
            )
        };
        match ret {
            0 => Ok(obj),
            _ => {
                bail!("could not create rist {:?}", obj.mode)
            }
        }
    }

    /// # Errors
    ///
    /// TODO: add error description
    pub fn connect(&mut self, p_url: &str) -> Result<()> {
        self.url = p_url.to_string();
        let ret = unsafe { w_rist_connect(self.ctx, self.url.as_ptr().cast::<i8>()) };
        match ret {
            0 => Ok(()),
            _ => {
                bail!("could not connect to {}", self.url)
            }
        }
    }

    /// # Errors
    ///
    /// TODO: add error description
    /// # Safety
    ///
    /// This function use void* for arg
    pub unsafe fn set_auth_handler(
        &self,
        p_arg: *mut c_void,
        p_on_auth_connect_cb: w_rist_auth_connect_callback,
        p_on_auth_disconnect_cb: w_rist_auth_disconnect_callback,
    ) -> Result<()> {
        let ret = w_rist_set_auth_handler(
            self.ctx,
            p_arg,
            p_on_auth_connect_cb,
            p_on_auth_disconnect_cb,
        );
        match ret {
            0 => Ok(()),
            _ => {
                bail!("could not set auth handler for {}", self.url)
            }
        }
    }

    /// # Errors
    ///
    /// TODO: add error description
    /// # Safety
    ///
    /// This function use void* for arg
    pub unsafe fn set_conn_status_callback(
        &self,
        p_arg: *mut c_void,
        p_on_callback: w_rist_connection_status_callback,
    ) -> Result<()> {
        let ret = w_rist_set_conn_status_callback(self.ctx, p_arg, p_on_callback);
        match ret {
            0 => Ok(()),
            _ => {
                bail!("could not set connection status callback for {}", self.url)
            }
        }
    }

    /// # Errors
    ///
    /// TODO: add error description
    /// # Safety
    ///
    /// This function use void* for arg
    pub unsafe fn set_out_of_band_callback(
        &self,
        p_arg: *mut c_void,
        p_on_callback: w_rist_out_of_band_callback,
    ) -> Result<()> {
        let ret = w_rist_set_out_of_band_callback(self.ctx, p_arg, p_on_callback);
        match ret {
            0 => Ok(()),
            _ => {
                bail!("could not set out of band callback for {}", self.url)
            }
        }
    }
    /// # Errors
    ///
    /// TODO: add error description
    /// # Safety
    ///
    /// This function use void* for arg
    pub unsafe fn set_stats_callback(
        &self,
        p_interval: i32,
        p_arg: *mut c_void,
        p_on_callback: w_rist_stats_callback,
    ) -> Result<()> {
        let ret = w_rist_set_stats_callback(self.ctx, p_interval, p_arg, p_on_callback);
        match ret {
            0 => Ok(()),
            _ => {
                bail!("could not set stats callback for {}", self.url)
            }
        }
    }

    /// # Errors
    ///
    /// TODO: add error description
    /// # Safety
    ///
    /// This function use void* for arg
    pub unsafe fn set_receiver_data_callback(
        &self,
        p_arg: *mut c_void,
        p_on_callback: w_receiver_data_callback,
    ) -> Result<()> {
        let ret = w_rist_set_receiver_data_callback(self.ctx, p_on_callback, p_arg);
        match ret {
            0 => Ok(()),
            _ => {
                bail!("could not set receiver callback for {}", self.url)
            }
        }
    }

    #[must_use]
    pub fn send(&self, p_data_block: &rist_data_block) -> i32 {
        unsafe { w_rist_write(self.ctx, p_data_block.block) }
    }

    pub fn read(&self, p_data_block: &mut rist_data_block, p_timeout: i32) -> i32 {
        unsafe { w_rist_read(self.ctx, &mut p_data_block.block, p_timeout) }
    }
}
