#[tokio::test]
async fn test_gametime() {
    use wolf::system::chrono::w_gametime::WGameTime;
    let mut gtime = WGameTime::new();
    gtime.set_fixed_time_step(true);
    gtime.set_target_elapsed_seconds(1.0 / 60.0); //ticks every 16 ms (60 fps)

    loop {
        gtime.tick_fn(move || {
            println!(
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
