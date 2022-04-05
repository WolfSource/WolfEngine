#![feature(async_closure)]
#![allow(unused_imports)]
#![allow(unreachable_code)]
use wasm_bindgen::prelude::*;
use wasm_mt::prelude::*;
use wolf::{
    system::{os::thread::WThread, script::javascript::WJavaScript},
    wlog,
};

async fn test() -> Result<JsValue, JsValue> {
    wlog!("hello from t2 worker");
    let r = JsValue::from_bool(true);
    Ok(r)
}

#[wasm_bindgen]
pub fn main() {
    WThread::spawn(
        FnOnce!(async move || {
            wlog!("t1 worker started");
            WThread::sleep(std::time::Duration::from_secs(5)).await;
            wlog!("t1 worker stoped after 5 seconds");
            Ok(JsValue::null())
        }),
        None,
    );
    WThread::spawn(FnOnce!(async move || test().await), None);

    let f = async move {
        let js = WJavaScript::new(None);
        let _js_res = js
            .execute(
                "
             console.log(\"hello from javascript promise\");
             const sub = (a, b) => new Promise(resolve => {
                 setTimeout(() => resolve(a - b), 1000);
             });
             return await sub(1, 2);
         "
                .to_owned(),
                true,
            )
            .await;
    };
    WThread::spawn_local(f);
}
