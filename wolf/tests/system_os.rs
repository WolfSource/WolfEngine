#![allow(unused_crate_dependencies)]

#[allow(clippy::too_many_lines)]
#[tokio::test]
async fn test_process() {
    use wolf::{
        system::os::{runtime::RunTime, system::System},
        w_log,
    };

    let path_to_the_process: &str;
    let process_name: &str;

    #[cfg(target_os = "windows")]
    {
        path_to_the_process = "C:/Windows/System32/NotePad.exe";
        process_name = "Notepad.exe";
    }
    #[cfg(target_os = "macos")]
    {
        path_to_the_process = "/System/Applications/TextEdit.app/Contents/MacOS/TextEdit";
        process_name = "TextEdit";
    }
    #[cfg(target_os = "linux")]
    {
        path_to_the_process = "TextEdit";
        process_name = "TextEdit";
    }

    let (sx, rx) = std::sync::mpsc::channel();

    // try to create a unknown process
    let unknown_process_name = "unknown";
    let mut sys = System::new();
    let p_res = System::create_process(unknown_process_name, &[]).await;
    assert!(
        p_res.is_err(),
        "error was expected on creating an unknown process"
    );
    assert!(
        !sys.is_process_running_by_name(unknown_process_name),
        "error was expected on checking unknown process"
    );

    #[cfg(target_os = "windows")]
    assert!(
        !sys.is_process_running_by_pid(usize::MAX),
        "error was expected on checking process_id = usize::MAX"
    );
    #[cfg(not(target_os = "windows"))]
    assert!(
        !sys.is_process_running_by_pid(i32::MAX),
        "error was expected on checking process_id = usize::MAX"
    );

    // check path
    if path_to_the_process.is_empty() {
        w_log!("process name is empty");
    } else {
        sys.refresh_disks();
        sys.refresh_networks();
        sys.refresh_processes();
        sys.refresh_system();

        w_log!(
            "Memory: {}KB/{}KB",
            sys.used_memory_in_kb(),
            sys.total_memory_in_kb()
        );
        w_log!(
            "Swap: {}KB/{}KB",
            sys.used_swap_in_kb(),
            sys.total_swap_in_kb()
        );

        // run process in one thread
        let join_1 = RunTime::green_thread(async move {
            let child = System::create_process(path_to_the_process, &[])
                .await
                .unwrap();
            RunTime::async_sleep(std::time::Duration::from_secs(3)).await;
            w_log!("process status {:?}", child);
            sx.send(child).expect("unable to send child process");
        });

        loop {
            if join_1.is_finished() {
                let mut sys = System::new();
                sys.refresh_all();

                w_log!(
                    "Memory: {}KB/{}KB",
                    sys.used_memory_in_kb(),
                    sys.total_memory_in_kb()
                );
                w_log!(
                    "Swap: {}KB/{}KB",
                    sys.used_swap_in_kb(),
                    sys.total_swap_in_kb()
                );

                let mut child = rx.recv().expect("could not receive an object");

                #[cfg(target_os = "windows")]
                let process_id = child.id().unwrap() as usize;

                #[cfg(not(target_os = "windows"))]
                let process_id = child.id().unwrap() as i32;

                println!(
                    "The number of Notepad process: {}",
                    sys.number_of_process_instances(process_name)
                );

                if sys.is_process_running_by_name(process_name) {
                    w_log!("Notepad.exe found");
                } else {
                    w_log!("could not find Notepad.exe");
                }

                // if process is avaiable then kill it
                if sys.is_process_running_by_pid(process_id) {
                    let killed_res = child.kill().await;
                    match killed_res {
                        Ok(_) => {
                            w_log!("process child just killed");
                        }
                        Err(err) => {
                            w_log!("could not kill process because {:?}", err);
                        }
                    };
                }
                break;
            }
            RunTime::async_sleep(std::time::Duration::from_millis(50)).await;
        }
    }
}

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
    let j_thread = RunTime::thread(move || {
        fun("os thread".to_owned());
    });
    // sleep for a sec
    RunTime::sleep(std::time::Duration::from_secs(1));
    // run the function on a green thread
    let j_green = RunTime::green_thread(async move { fun("green thread".to_owned()) });

    // run a future on local thread
    RunTime::spawn_local(async {
        w_log!("future spawned on the local thread");
    });
    // wait for all joinables
    j_thread.join().unwrap();
    j_green.await.unwrap();
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
    .await
    .unwrap();

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

#[cfg(feature = "system_screen_capture")]
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
