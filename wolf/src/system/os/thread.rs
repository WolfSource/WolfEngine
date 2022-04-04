use std::future::Future;

#[cfg(feature = "wasm")]
use {
    crate::wlog,
    serde::{de::DeserializeOwned, Serialize},
    wasm_bindgen::JsValue,
    wasm_mt::{MtAsyncClosure, WasmMt},
};

#[cfg(not(feature = "wasm"))]
use tokio::task::JoinHandle;

pub struct WThread {}

impl WThread {
    #[cfg(feature = "wasm")]
    pub fn spawn<F, T>(p_fn: F, p_path_to_pkg_js: Option<String>)
    where
        F: FnOnce() -> T + Serialize + DeserializeOwned + 'static,
        T: Future<Output = Result<JsValue, JsValue>> + 'static,
    {
        const TRACE: &str = "WThread::spawn";

        let path_to_pkg_js = if p_path_to_pkg_js.is_some() {
            p_path_to_pkg_js.unwrap_or_default()
        } else {
            "./pkg/wolf_demo.js".to_owned()
        };

        let f = async move {
            // init the wasm web worker
            let wasm_mt_res = WasmMt::new(&path_to_pkg_js).and_init().await;
            match wasm_mt_res {
                Ok(wasm_mt) => {
                    // init a thread from web worker
                    let thread_res = wasm_mt.thread().and_init().await;
                    match thread_res {
                        Ok(t) => {
                            // execute async closure with web worker
                            let _ = t.exec_async(p_fn).await;
                        }
                        Err(e) => {
                            wlog!("{:?}. trace info: {}", e, TRACE);
                        }
                    };
                }
                Err(e) => {
                    wlog!("{:?}. trace info: {}", e, TRACE);
                }
            };
        };
        wasm_bindgen_futures::spawn_local(f);
    }
    #[cfg(feature = "wasm")]
    pub async fn sleep(p_duration: std::time::Duration) {
        wasm_mt::utils::sleep(p_duration.as_millis() as u32).await;
    }

    #[cfg(not(feature = "wasm"))]
    pub async fn spawn<F, R>(p_func: F) -> JoinHandle<F::Output>
    where
        F: Future + Send + 'static,
        F::Output: Send + 'static,
    {
        tokio::spawn(p_func)
    }

    #[cfg(not(feature = "wasm"))]
    pub async fn sleep(p_duration: std::time::Duration) {
        tokio::time::sleep(p_duration).await;
    }
}
