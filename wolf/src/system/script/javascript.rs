// use wasm_bindgen::JsValue;
// use wasm_mt::{exec_js, exec_js_async, WasmMt};

// pub struct JavaScript {
//     js_worker_path: String,
// }

// impl JavaScript {
//     pub fn new(p_path_to_js_worker: Option<String>) -> Self {
//         let path_to_pkg_js = if p_path_to_js_worker.is_some() {
//             p_path_to_js_worker.unwrap_or_default()
//         } else {
//             "./pkg/wolf_demo.js".to_owned()
//         };
//         Self {
//             js_worker_path: path_to_pkg_js,
//         }
//     }

//     pub async fn execute(&self, p_js_script: String, p_async: bool) -> Result<JsValue, JsValue> {
//         // init the wasm web worker
//         let wasm_mt_res = WasmMt::new(&self.js_worker_path).and_init().await;

//         match wasm_mt_res {
//             Ok(wasm_mt) => {
//                 // init a thread from web worker
//                 let thread_res = wasm_mt.thread().and_init().await;

//                 match thread_res {
//                     Ok(t) => {
//                         // execute async closure with web worker
//                         let js_res = if p_async {
//                             exec_js_async!(t, &p_js_script).await
//                         } else {
//                             exec_js!(t, &p_js_script).await
//                         };
//                         match js_res {
//                             Ok(js_val) => Ok(js_val),
//                             Err(e) => Err(e),
//                         }
//                     }
//                     Err(e) => Err(e),
//                 }
//             }
//             Err(e) => Err(e),
//         }
//     }
// }
