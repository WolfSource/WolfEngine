use std::time::Duration;

pub async fn timout<T>(p_duration: Duration, p_future: T) -> anyhow::Result<()>
where
    T: core::future::Future,
{
    if let Err(e) = tokio::time::timeout(p_duration, p_future).await {
        anyhow::bail!("timout reached after {}", e)
    }
    Ok(())
}

pub async fn timer<F, R>(p_interval: Duration, mut p_call_back: F) -> anyhow::Result<R>
where
    F: FnMut(
        &f64, /* delta time in seconds */
        &f64, /* total time in seconds */
        &mut bool,
    ) -> anyhow::Result<R>,
{
    let mut interval = tokio::time::interval(p_interval);
    let mut cancel = false;
    let base_time = interval.tick().await;
    let mut last_time = base_time.clone();
    loop {
        let current_time = interval.tick().await;
        let delta_time = current_time - last_time;
        let total_time = current_time - base_time;
        last_time = current_time;

        let ret = p_call_back(
            &delta_time.as_secs_f64(),
            &total_time.as_secs_f64(),
            &mut cancel,
        );
        if cancel {
            return ret;
        }
    }
}

pub async fn timer_oneshot<F>(
    p_interval: Duration,
    p_future: F,
) -> <F as core::future::Future>::Output
where
    F: core::future::Future,
{
    let mut interval = tokio::time::interval(p_interval);
    interval.tick().await;
    p_future.await
}

pub async fn sleep(p_duration: Duration) -> () {
    tokio::time::sleep(p_duration).await
}

#[tokio::main]
#[test]
async fn test() -> std::result::Result<(), Box<dyn std::error::Error>> {
    //test timeout for future
    let f = async { tokio::time::sleep(Duration::from_secs(5)).await };
    let ret = timout(Duration::from_secs(2), f).await;
    assert!(ret.is_err());

    //launch a oneshot timer with 3 seconds
    let _ = timer_oneshot(Duration::from_secs(3), async {
        println!("oneshot timer ticked! and callback is going to sleep for 1 second");
        sleep(Duration::from_secs(1)).await;
        println!("oneshot timer done!");

        Ok(()) as anyhow::Result<()>
    })
    .await?;

    //launch a timer with 1 seconds interval
    let ret = timer(
        Duration::from_secs(1),
        move |p_delta_time_in_secs: &f64,
              p_total_time_in_secs: &f64,
              p_cancel: &mut bool|
              -> anyhow::Result<f64> {
            println!(
                "timer ticked! delta_time_in_secs is {} total_time_in_secs {}",
                p_delta_time_in_secs, p_total_time_in_secs
            );

            // after 10 seconds, cancel timer
            if p_total_time_in_secs > &4.0 {
                *p_cancel = true;
            }

            Ok(*p_total_time_in_secs)
        },
    )
    .await?;

    println!("timer cancelled after {} seconds", ret);

    Ok(())
}
