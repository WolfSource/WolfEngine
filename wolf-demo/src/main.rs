#![feature(async_closure)]
#![allow(unused_imports)]
#![allow(unreachable_code)]

use wolf::{
    system::{os::w_runtime::WRuntime, script::w_rhai::WRhai},
    w_log,
};

#[cfg(target_arch = "wasm32")]
use {
    wasm_bindgen::prelude::*, wasm_mt::prelude::*, wolf::system::script::w_javascript::WJavaScript,
};

// Normal function
fn add(x: i64, y: i64) -> i64 {
    x + y
}

async fn app() {
    let mut script = WRhai::new();

    // register add function for our embedded script
    script.register_function("add", add);

    let res = script.run_return_any::<i64>(r#"add(10, 7)"#);
    match res {
        Ok(v) => {
            w_log!("add returns: {}", v);
        }
        Err(e) => {
            w_log!("add returns error: {:?}", e);
        }
    };

    #[cfg(not(target_arch = "wasm32"))]
    {
        let f = async move || {
            println!("t1 started");
            WRuntime::sleep(std::time::Duration::from_secs(1));
            w_log!("t1 just stopped after 2 seconds");
        };
        // execute thread
        WRuntime::green_thread(f()).await;
        WRuntime::async_sleep(std::time::Duration::from_secs(2)).await;
    }

    #[cfg(target_arch = "wasm32")]
    {
        let f1 = async move {
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
        WRuntime::spawn_local(f1);

        #[cfg(target_arch = "wasm32")]
        let f2 = FnOnce!(async move || {
            w_log!("t1 worker started");
            WRuntime::async_sleep(std::time::Duration::from_secs(2)).await;
            w_log!("t1 worker just stopped after 5 seconds");
            Ok(JsValue::null())
        });
        // execute thread
        WRuntime::thread(f2);
    }

    use wolf::system::os::w_runtime::WRuntime;
    use wolf::system::os::w_sigslot::WSigSlot;
    use wolf::w_log;

    // create SigSlot
    let mut sig_slot = WSigSlot::new();

    // create slots
    let i = 1;
    let con_1 = sig_slot.connect(move || {
        w_log!("hello from slot{}", i);
    });
    let con_2 = sig_slot.connect(|| {
        w_log!("hello from slot2");
    });

    // check for connections
    if con_1.is_connected() && con_2.is_connected() {
        w_log!("slot 1 & 2 was connected");
        // disconnect slot 2
        con_2.disconnect();
        w_log!("slot 2 just disconnected");
    }

    // wait for threads
    WRuntime::async_sleep(std::time::Duration::from_secs(1)).await;
    // emit all
    sig_slot.emit();
}

#[cfg(target_arch = "wasm32")]
#[wasm_bindgen]
pub fn main() {
    w_log!("starting wolf-demo in wasm mode");
    WRuntime::spawn_local(async {
        app().await;
    });
}

#[cfg(not(target_arch = "wasm32"))]
#[tokio::main]
pub async fn main() {
    w_log!("starting wolf-demo in native mode");
    app().await;
}
