use std::time::Duration;

pub mod interval;
use interval::Interval;

// mod timeout;
// /// A timeout handler function
// /// # Arguments
// ///
// /// * `p_duration` - the interval of timeout
// /// * `p_future` - the future which will execute
// ///
// /// # Errors
// /// returns Err when timeout reached
// ///
// /// # Cancellation
// ///
// /// Cancelling a timeout is done by dropping the future. No additional cleanup
// /// or other work is required.
// ///
// /// Example:
// ///
// /// ```no_run
// /// use wolf_system::chrono::time;
// /// use std::time::Duration;
// /// let func = async { tokio::time::sleep(Duration::from_secs(5)).await };
// /// let mut ret = time::timeout(Duration::from_secs(2), func).await;
// /// assert!(ret.is_err()); //timeout should return error///
// /// ```
// pub async fn timeout<F>(p_duration: Duration, p_future: F) -> anyhow::Result<()>
// where
//     F: Future<Output = ()> + Send + 'static,
// {
//     // Don't forget to add this to Cargo dependencies before uncommenting!
//     // futures = { version = "0.3.21", default-features = false, features= ["alloc", "async-await", "std"] }

//     timeout::timeout(p_duration, p_future).await
// }

/// A timer with interval and callback function
///
/// # Arguments
///
/// * `p_interval` - the interval of timer
/// * `p_future` - the function which will be executed in each interval
///
/// # Errors
/// returns Err which comes from callback function
///
/// Example:
///
/// ```no_run
/// use wolf_system::chrono::time;
///
/// let res_timer = time::timer(
///     Duration::from_secs(1),
///     move |p_delta_time_in_secs: f64,
///           p_total_time_in_secs: f64,
///           p_cancel: &mut bool|
///           -> anyhow::Result<f64> {
///         println!(
///             "timer ticked! delta_time_in_secs is {} total_time_in_secs {}",
///             p_delta_time_in_secs, p_total_time_in_secs
///         );
///         // after 4 seconds, cancel the timer
///         if p_total_time_in_secs > 4.0 {
///             *p_cancel = true;
///         }
///         Ok(p_total_time_in_secs)
///     },
/// ).await;
/// ```
pub async fn timer<F, R>(p_interval: Duration, mut p_call_back: F) -> anyhow::Result<R>
where
    F: FnMut(
            f64, /* delta time in seconds */
            f64, /* total time in seconds */
            &mut bool,
        ) -> anyhow::Result<R>
        + Send,
{
    let mut interval = Interval::new(p_interval, interval::cross_platform_sleep);

    let mut cancel = false;

    let base_time = interval.tick().await;

    let mut last_time = base_time;

    loop {
        let current_time = interval.tick().await;

        let delta_time = current_time - last_time;
        let total_time = current_time - base_time;

        last_time = current_time;

        let ret = p_call_back(
            delta_time.as_secs_f64(),
            total_time.as_secs_f64(),
            &mut cancel,
        );

        if cancel {
            return ret;
        }
    }
}

/// A timer with interval and callback function
///
/// # Arguments
///
/// * `p_interval` - the interval of timer
/// * `p_future` - the future which should be executed
///
/// Example:
///
/// ```no_run
/// use wolf_system::chrono::time;
///
/// //launch a oneshot timer with 3 seconds
/// let ret = time::timer_oneshot(Duration::from_secs(3), async {
///     println!("oneshot timer ticked! This message comes from the callback function.");
///     Ok(())
/// })
/// .await;
/// ```
pub async fn timer_oneshot<F>(
    p_interval: Duration,
    p_future: F,
) -> <F as core::future::Future>::Output
where
    F: core::future::Future,
{
    interval::cross_platform_sleep(p_interval.as_millis() as u64).await;

    p_future.await
}

#[tokio::main]
#[cfg(not(target_arch = "wasm32"))]
#[test]
async fn test() {
    let oneshot_task = async {
        println!("oneshot timer ticked! The callback function is going to sleep for 1 second.");

        interval::cross_platform_sleep(1000).await;

        println!("oneshot timer done!");
    };

    //launch a oneshot timer with 3 seconds
    timer_oneshot(Duration::from_secs(3), oneshot_task).await;

    //launch a timer with 1 seconds interval
    let res_timer = timer(
        Duration::from_secs(1),
        move |p_delta_time_in_secs: f64,
              p_total_time_in_secs: f64,
              p_cancel: &mut bool|
              -> anyhow::Result<f64> {
            println!(
                "timer ticked! delta_time_in_secs is {} total_time_in_secs {}",
                p_delta_time_in_secs, p_total_time_in_secs
            );

            // after 4 seconds, cancel the timer
            if p_total_time_in_secs > 4.0 {
                *p_cancel = true;
            }

            Ok(p_total_time_in_secs)
        },
    )
    .await;
    assert!(res_timer.is_ok());

    println!("timer just cancelled after {:?} seconds", res_timer);
}
