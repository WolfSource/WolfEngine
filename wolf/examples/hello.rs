#![allow(unused_crate_dependencies)]

use wolf::system::chrono::gametime::GameTime;
use wolf::{render::scene, wlog};

fn main() {
    //This is a simple test and should be run on both Native & WASM

    let mut gtime = GameTime::new();
    gtime.set_fixed_time_step(true);
    gtime.set_target_elapsed_seconds(1.0 / 60.0); //ticks every 16 ms (60 fps)

    scene::start();
    loop {
        gtime.tick_fn(move || {
            wlog!(
                "elapsed seconds from last tick {}. total elapsed seconds {}",
                gtime.get_elapsed_seconds(),
                gtime.get_total_elapsed_seconds(),
            );
        });

        // break after 5 seconds
        if gtime.get_total_elapsed_seconds() > 5.0 {
            break;
        }
    }
}
