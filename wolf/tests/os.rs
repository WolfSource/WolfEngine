#[tokio::test]
async fn test_runtime() {
    use wolf::system::os::w_runtime::WRuntime;
    use wolf::w_log;

    // declare a function
    let f = move |p_msg: String| {
        w_log!("hello from {}", &p_msg);
        true
    };
    // run the function on a OS thread
    WRuntime::thread(move || {
        f("os thread".to_owned());
    });
    // sleep for a sec
    WRuntime::sleep(std::time::Duration::from_secs(1));
    // run the function on a green thread
    WRuntime::green_thread(async move { f("green thread".to_owned()) }).await;
    // wait for all
    WRuntime::async_sleep(std::time::Duration::from_secs(5)).await;
}

#[tokio::test]
async fn tes_sigslot() {
    use wolf::system::os::w_runtime::WRuntime;
    use wolf::system::os::w_sigslot::WSigSlot;
    use wolf::w_log;

    // create SigSlot
    let mut sig_slot = WSigSlot::new();
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
    let _j = WRuntime::thread(move || {
        sig_cloned_1.connect(|| {
            w_log!("hello from slot of os thread");
        });
    });
    WRuntime::green_thread(async move {
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
    WRuntime::async_sleep(std::time::Duration::from_secs(1)).await;
    // emit all
    sig_slot.emit();
}
