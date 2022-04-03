#[cfg(target_arch = "wasm32")]
use {
    anyhow::{bail, Result},
    wasm_bindgen::JsValue,
    wasm_mt::{exec, prelude::*, MtAsyncClosure, WasmMt},
};

#[cfg(not(target_arch = "wasm32"))]
use tokio::task::JoinHandle;

pub struct WThread {}

impl WThread {
    #[cfg(target_arch = "wasm32")]
    pub async fn spawn<F, T>(p_func: F, p_path_to_pkg_js: Option<&str>) -> Result<JsValue>
    where
        F: MtAsyncClosure<T>,
    {
        const TRACE: &str = "WThread::spawn";

        let path_to_pkg_js = if p_path_to_pkg_js.is_some() {
            p_path_to_pkg_js.unwrap_or_default()
        } else {
            "./pkg/wolf_demo.js"
        };

        // init the wasm web worker
        let wasm_mt_res = WasmMt::new(path_to_pkg_js).and_init().await;
        let ret = match wasm_mt_res {
            Ok(wasm_mt) => {
                // init a thread from web worker
                let thread_res = wasm_mt.thread().and_init().await;
                let ret = match thread_res {
                    Ok(t) => {
                        // execute async closure with web worker
                        let js_value_res = exec!(t, async move || p_func.call_once(()).await).await;
                        match js_value_res {
                            Ok(val) => Ok(val),
                            Err(e) => bail!("{:?}. trace info: {}", e, TRACE),
                        }
                    }
                    Err(e) => {
                        bail!("{:?}. trace info: {}", e, TRACE)
                    }
                };
                ret
            }
            Err(e) => bail!("{:?}. trace info: {}", e, TRACE),
        };
        ret
    }

    #[cfg(not(target_arch = "wasm32"))]
    pub async fn spawn<F, R>(p_func: F) -> JoinHandle<F::Output>
    where
        F: std::future::Future + Send + 'static,
        F::Output: Send + 'static,
    {
        tokio::spawn(p_func)
    }
}
