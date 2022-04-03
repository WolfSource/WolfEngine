#![allow(unused_crate_dependencies)]
use wolf::{render::scene, wlog};

lazy_static::lazy_static! {
    static ref C : std::sync::Mutex<(std::sync::mpsc::Sender<bool>, std::sync::mpsc::Receiver<bool>)> = std::sync::Mutex::from(std::sync::mpsc::channel::<bool>());
}

pub async fn F1() {
    wlog!("running task1");
    let mut gtime = wolf::system::chrono::gametime::GameTime::new();
    loop {
        gtime.tick();
        wlog!("task1...");
        // break after 5 seconds
        if gtime.get_total_elapsed_seconds() > 1.0 {
            C.try_lock().unwrap().0.send(true).unwrap();
            wlog!("task1 breaks task2");
            break;
        }
    }
    wlog!("task1 done");
}

pub async fn F2() {
    wlog!("running task2");
    let mut gtime = wolf::system::chrono::gametime::GameTime::new();
    loop {
        let close_res = C.try_lock().map(|m| {
            let mm = m.1.recv().unwrap();
            mm
        });

        if close_res.is_ok() {
            wlog!("task2 breaks");
            break;
        }

        gtime.tick();
        wlog!("task2...");
        // break after 5 seconds
        if gtime.get_total_elapsed_seconds() > 3.0 {
            break;
        }
    }
    wlog!("task2 done");
}

fn main() {
    //This is a simple test and should be run on both Native & WASM
    //wasm_bindgen_futures::spawn_local(F1());
    //wasm_bindgen_futures::spawn_local(F2());

    // use wolf::system::os::wthread;
    // wthread::default_thread_pool(None);

    // use std::time::Duration;
    // use wolf::system::chrono::time::*;

    // wlog!("done");
    // let v = rx.recv();
    // wlog!("result is {:?}", v);

    // let mut gtime = GameTime::new();
    // gtime.set_fixed_time_step(true);
    // gtime.set_target_elapsed_seconds(1.0 / 60.0); //ticks every 16 ms (60 fps)

    wlog!("HERE");
    scene::start();
}
