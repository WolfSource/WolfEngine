use crate::{
    stream::ffi::rist::{
        size_t, w_buf, w_buf_t, w_rist, w_rist_bind, w_rist_config_t, w_rist_fini,
        w_rist_is_stopped, w_rist_mode_W_RIST_MODE_RECEIVER, w_rist_profile_W_RIST_PROFILE_SIMPLE,
        w_rist_start, w_rist_stop,
    },
    w_log, MAX_TRACE_BUFFER_SIZE,
};
use anyhow::{anyhow, bail, Result};
use std::{ffi::CString, os::raw::c_char};

#[cfg(all(not(target_arch = "wasm32"), feature = "stream_rist"))]
pub struct rist {
    pub config: w_rist_config_t,
    pub core: w_rist,
}

impl Drop for rist {
    fn drop(&mut self) {
        if !self.core.is_null() {
            let ret = unsafe { w_rist_fini(&mut self.core) };
            if ret != 0 {
                w_log!("could not drop rist");
            }
        }
    }
}

impl Default for rist {
    fn default() -> Self {
        Self::new(w_rist_config_t {
            mode: w_rist_mode_W_RIST_MODE_RECEIVER,
            profile: w_rist_profile_W_RIST_PROFILE_SIMPLE,
            loss_percent: 0,
            timeout: 0,
        })
    }
}

impl rist {
    pub fn new(p_config: w_rist_config_t) -> Self {
        Self {
            config: p_config,
            core: std::ptr::null_mut(),
        }
    }

    pub fn bind(&mut self, p_url: String) -> Result<()> {
        const TRACE: &str = "rist::bind";

        // cast String to *const c_char
        let c_str = CString::new(p_url).map_err(|e| {
            anyhow!(
                "could not convert p_url to CString because of {:?}. trace info: {}",
                e,
                TRACE
            )
        })?;
        let c_url = c_str.as_ptr() as *const c_char;

        let mut trace_data = [0u8; MAX_TRACE_BUFFER_SIZE];
        let mut trace_buffer = w_buf_t {
            data: trace_data.as_mut_ptr(),
            len: MAX_TRACE_BUFFER_SIZE as size_t,
        };
        let trace_buffer_ptr = &mut trace_buffer as w_buf;

        let ret = unsafe { w_rist_bind(&mut self.core, c_url, &mut self.config, trace_buffer_ptr) };
        if ret == 0 {
            Ok(())
        } else {
            let e = unsafe { std::ffi::CStr::from_ptr(trace_buffer.data as *mut c_char) };
            bail!(
                "could not bind rist because of {:?}. trace info: {}",
                e,
                TRACE
            )
        }
    }

    pub fn start(&self) -> Result<()> {
        const TRACE: &str = "rist::start";

        // create trace log
        let mut trace_data = [0u8; MAX_TRACE_BUFFER_SIZE];
        let mut trace_buffer = w_buf_t {
            data: trace_data.as_mut_ptr(),
            len: MAX_TRACE_BUFFER_SIZE as size_t,
        };
        let trace_buffer_ptr = &mut trace_buffer as w_buf;

        // start rist
        let ret = unsafe { w_rist_start(self.core, trace_buffer_ptr) };
        if ret == 0 {
            Ok(())
        } else {
            let e = unsafe { std::ffi::CStr::from_ptr(trace_buffer.data as *mut c_char) };
            bail!(
                "could not start rist because of {:?}. trace info: {}",
                e,
                TRACE
            )
        }
    }

    pub fn stop(&self) -> Result<()> {
        const TRACE: &str = "rist::stop";

        // start rist
        let ret = unsafe { w_rist_stop(self.core) };
        if ret == 0 {
            Ok(())
        } else {
            bail!("could not stop rist. trace info: {}", TRACE)
        }
    }

    pub fn is_stopped(&self) -> bool {
        unsafe { w_rist_is_stopped(self.core) }
    }
}
