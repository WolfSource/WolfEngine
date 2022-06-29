// Normal function
const fn add(x: i64, y: i64) -> i64 {
    x + y
}

#[tokio::test]
async fn test_rhai() {
    use wolf::{
        system::{os::runtime::RunTime, script::rhai::Rhai},
        w_log,
    };

    let mut script = Rhai::new();

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

    let f = async move {
        println!("t1 started");
        RunTime::sleep(std::time::Duration::from_secs(1));
        w_log!("t1 just stopped after 2 seconds");
    };
    // execute thread
    RunTime::green_thread(f).await;
    RunTime::async_sleep(std::time::Duration::from_secs(2)).await;
}
