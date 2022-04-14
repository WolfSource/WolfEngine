#![feature(async_closure)]
#![allow(unused_imports)]
#![allow(unreachable_code)]
use anyhow::Result;
use wolf::system::os::w_sigslot::WSigSlot;
use wolf::{
    render::{
        w_graphics_device::WGraphicsDevice,
        w_scene::{IWScene, WSceneManager},
    },
    system::{chrono::w_gametime::WGameTime, os::w_runtime::WRunTime, script::w_rhai::WRhai},
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

async fn test() {
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
            WRunTime::sleep(std::time::Duration::from_secs(1));
            w_log!("t1 just stopped after 2 seconds");
        };
        // execute thread
        WRunTime::green_thread(f()).await;
        WRunTime::async_sleep(std::time::Duration::from_secs(2)).await;
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
        WRunTime::spawn_local(f1);

        #[cfg(target_arch = "wasm32")]
        let f2 = FnOnce!(async move || {
            w_log!("t1 worker started");
            WRunTime::async_sleep(std::time::Duration::from_secs(2)).await;
            w_log!("t1 worker just stopped after 5 seconds");
            Ok(JsValue::null())
        });
        // execute thread
        WRunTime::thread(f2);
    }

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
    WRunTime::async_sleep(std::time::Duration::from_secs(1)).await;
    // emit all
    sig_slot.emit();
}

#[derive(Default)]
struct WScene {}

impl IWScene for WScene {
    fn load(&self, _p_g_device: &mut WGraphicsDevice) -> Result<()> {
        w_log!("scene is going to loaded");
        WRunTime::spawn_local(async move {
            test().await;
        });
        w_log!("scene just loaded");
        Ok(())
    }
    fn render(&self, _p_g_device: &mut WGraphicsDevice, _p_gametime: &mut WGameTime) -> Result<()> {
        w_log!("scene is rendering");
        Ok(())
    }
}

#[cfg(target_arch = "wasm32")]
#[wasm_bindgen]
pub fn main() {
    // run scene
    w_log!("starting wolf-demo in wasm mode");
    WRunTime::spawn_local(async move {
        let _res = WSceneManager::run(WScene::default()).await;
    });
}

#[cfg(not(target_arch = "wasm32"))]
#[tokio::main]
pub async fn main() {
    // run scene
    w_log!("starting wolf-demo in native mode");
    let _res = WSceneManager::run(WScene::default()).await;
}
