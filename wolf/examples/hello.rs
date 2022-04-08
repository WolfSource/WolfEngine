#![allow(unused_crate_dependencies)]
use wolf::{
    render::scene,
    system::{chrono::w_gametime::WGameTime, os::w_runtime::WRuntime},
    w_log,
};

lazy_static::lazy_static! {
    static ref C : std::sync::Mutex<(std::sync::mpsc::Sender<bool>, std::sync::mpsc::Receiver<bool>)> = std::sync::Mutex::from(std::sync::mpsc::channel::<bool>());
}

pub async fn f1() {
    w_log!("running task1");
    let mut gtime = WGameTime::new();
    loop {
        gtime.tick();
        w_log!("task1...");
        // break after 5 seconds
        if gtime.get_total_elapsed_seconds() > 1.0 {
            C.try_lock().unwrap().0.send(true).unwrap();
            w_log!("task1 breaks task2");
            break;
        }
    }
    w_log!("task1 done");
}

pub async fn f2() {
    w_log!("running task2");
    let mut gtime = wolf::system::chrono::w_gametime::WGameTime::new();
    loop {
        let close_res = C.try_lock().map(|m| m.1.recv().unwrap());

        if close_res.is_ok() {
            w_log!("task2 breaks");
            break;
        }

        gtime.tick();
        w_log!("task2...");
        // break after 5 seconds
        if gtime.get_total_elapsed_seconds() > 3.0 {
            break;
        }
    }
    w_log!("task2 done");
}

fn main() {
    //This is a simple test and should be run on both Native & WASM
    WRuntime::spawn_local(f1());
    WRuntime::spawn_local(f2());

    // use wolf::system::os::wthread;
    // wthread::default_thread_pool(None);

    // use std::time::Duration;
    // use wolf::system::chrono::time::*;

    // w_log!("done");
    // let v = rx.recv();
    // w_log!("result is {:?}", v);

    // let mut gtime = GameTime::new();
    // gtime.set_fixed_time_step(true);
    // gtime.set_target_elapsed_seconds(1.0 / 60.0); //ticks every 16 ms (60 fps)

    w_log!("HERE");
    scene::start();
}
