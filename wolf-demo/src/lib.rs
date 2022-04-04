#![feature(async_closure)]

use wasm_bindgen::prelude::*;
use wasm_mt::{prelude::*, utils::console_ln};
use wolf::{system::os::thread::WThread, wlog};

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

    // let f1 = async move {
    //     let mt = WasmMt::new(pkg_js).and_init().await.unwrap();
    //     let th = mt.thread().and_init().await.unwrap();
    //     let _ = run_closure(&th).await;
    // };
    // let f2 = async move {
    //     let l = FnOnce!(move || { Ok(test()) });
    //     let _x = spawn(l).await;
    //     //let _ = run_async_closure(&th).await;
    // };

    // let f3 = async move {
    //     let mt = WasmMt::new(pkg_js).and_init().await.unwrap();
    //     let th = mt.thread().and_init().await.unwrap();
    //     let _ = run_js(&th).await;
    // };
    // let f4 = async move {
    //     let mt = WasmMt::new(pkg_js).and_init().await.unwrap();
    //     let th = mt.thread().and_init().await.unwrap();
    //     let _ = run_async_js(&th).await;
    // };

    // spawn_local(f1);

    // spawn_local(f3);
    // spawn_local(f4);
}

// fn add(a: i32, b: i32) -> i32 {
//     a + b
// }

// async fn run_closure(th: &Thread) -> Result<(), JsValue> {
//     let a = 1;
//     let b = 2;
//     let ans = exec!(th, move || {
//         let c = add(a, b);
//         Ok(JsValue::from(c))
//     })
//     .await?;
//     console_ln!("run_closure ans: {:?}", ans);
//     Ok(())
// }

// use wasm_mt::utils::sleep;
// async fn sub(a: i32, b: i32) -> i32 {
//     sleep(1000).await;
//     a - b
// }

// async fn run_async_closure(th: &Thread) -> Result<(), JsValue> {
//     let a = 1;
//     let b = 2;

//     let ans = exec!(th, async move || {
//         let c = sub(a, b).await;
//         Ok(JsValue::from(c))
//     })
//     .await?;

//     assert_eq!(ans, JsValue::from(-1));
//     console_ln!("run_async_closure ans: {:?}", ans);

//     Ok(())
// }

// async fn run_js(th: &Thread) -> Result<(), JsValue> {
//     let ans = exec_js!(
//         th,
//         "
//         const add = (a, b) => a + b;
//         return add(1, 2);
//     "
//     )
//     .await?;
//     assert_eq!(ans, JsValue::from(3));
//     console_ln!("run_js ans: {:?}", ans);

//     Ok(())
// }

// //

// async fn run_async_js(th: &Thread) -> Result<(), JsValue> {
//     let ans = exec_js_async!(
//         th,
//         "
//         const sub = (a, b) => new Promise(resolve => {
//             setTimeout(() => resolve(a - b), 1000);
//         });
//         return await sub(1, 2);
//     "
//     )
//     .await?;
//     assert_eq!(ans, JsValue::from(-1));
//     console_ln!("run_async_js ans: {:?}", ans);

//     Ok(())
// }
