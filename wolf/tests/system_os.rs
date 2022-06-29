#![allow(unused_crate_dependencies)]

#[tokio::test]
async fn test_runtime() {
    use wolf::system::os::runtime::RunTime;
    use wolf::w_log;

    // declare a function
    let fun = move |p_msg: String| {
        w_log!("hello from {}", &p_msg);
        true
    };
    // run the function on a OS thread
    RunTime::thread(move || {
        fun("os thread".to_owned());
    });
    // sleep for a sec
    RunTime::sleep(std::time::Duration::from_secs(1));
    // run the function on a green thread
    RunTime::green_thread(async move { fun("green thread".to_owned()) }).await;
    // wait for all
    RunTime::async_sleep(std::time::Duration::from_secs(5)).await;
}

#[tokio::test]
async fn test_sigslot() {
    use wolf::system::os::{runtime::RunTime, sigslot::SigSlot};
    use wolf::w_log;

    // create SigSlot
    let mut sig_slot = SigSlot::new();
    // make two clones for os & green threads
    let sig_cloned_1 = sig_slot.clone();
    let sig_cloned_2 = sig_slot.clone();

    // create slots
    let i = 1;
    let con_1 = sig_slot.connect(move || {
        w_log!("hello from slot{}", i);
    });
    let con_2 = sig_slot.connect(|| {
        w_log!("hello from slot2");
    });
    let _j = RunTime::thread(move || {
        sig_cloned_1.connect(|| {
            w_log!("hello from slot of os thread");
        });
    });
    RunTime::green_thread(async move {
        sig_cloned_2.connect(|| {
            w_log!("hello from slot of green thread");
        });
    })
    .await;
    // check for connections
    if con_1.is_connected() && con_2.is_connected() {
        w_log!("slot 1 & 2 was connected");
        // disconnect slot 2
        con_2.disconnect();
        w_log!("slot 2 just disconnected");
    }

    // wait for threads
    RunTime::async_sleep(std::time::Duration::from_secs(1)).await;
    // emit all
    sig_slot.emit();
}

#[test]
fn test_screen_captures() {
    use wolf::system::os::screen::Screen;
    use wolf::w_log;

    let mut screen = Screen::new_primary().unwrap();
    assert!(screen.width != 0);
    assert!(screen.height != 0);

    let mut i: u32 = 0;
    loop {
        let frame_res = screen.capture();
        match frame_res {
            Ok(frame) => {
                i += 1;
                w_log!("length of frame #{} is {}", i, frame.len());
            }
            Err(e) => {
                if e.kind() == std::io::ErrorKind::WouldBlock {
                    // Keep spinning.
                    const ONE_SEC: std::time::Duration = std::time::Duration::from_secs(1);
                    let one_frame = ONE_SEC / 60;
                    std::thread::sleep(one_frame);
                    continue;
                }
                panic!("could not capture: {}", e);
            }
        }

        if i > 5 {
            break;
        }
    }
}
