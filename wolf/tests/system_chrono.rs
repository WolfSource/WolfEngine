#![allow(unused_crate_dependencies)]

#[test]
fn test_gametime() {
    println!("wolf_sys version is : {:?}", wolf::sys_version());

    let mut gtime = wolf::system::chrono::gametime::GameTime::new();
    gtime.set_fixed_time_step(true);
    gtime.set_target_elapsed_seconds(1.0 / 60.0); //ticks every 16 ms (60 fps)

    loop {
        gtime.tick_fn(move || {
            println!(
                "elapsed seconds from last tick {} sec. total elapsed seconds {} sec",
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
