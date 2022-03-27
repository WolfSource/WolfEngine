use futures::{select, Future, FutureExt};
use std::{pin::Pin, time::Duration};

//use crate::system::macros::wlog;

/// A timeout handler function
/// # Arguments
///
/// * `p_duration` - the interval of timeout
/// * `p_future` - the future which will execute
///
/// # Errors
/// returns Err when timeout reached
///
/// # Cancellation
///
/// Cancelling a timeout is done by dropping the future. No additional cleanup
/// or other work is required.
///
/// Example:
///
/// ```no_run
/// use wolf::chrono::time;
/// use std::time::Duration;
/// let func = async { std::thread::sleep(Duration::from_secs(5)).await };
/// let mut ret = time::timeout(Duration::from_secs(2), func).await;
/// assert!(ret.is_err()); //timeout should return error///
/// ```
pub async fn timeout<F>(p_duration: Duration, p_future: F) -> anyhow::Result<()>
where
    F: Future + Send,
{
    let sleep_future = async {
        std::thread::sleep(p_duration);
        anyhow::bail!("timeout reached after {:?}", p_duration)
    };
    let res = select! {
        _ret1 = p_future.fuse() =>
        {
            Ok(())
        },
        ret2 = sleep_future.fuse() => ret2,
    };
    res
}

// /// A timer with interval and callback function
// ///
// /// # Arguments
// ///
// /// * `p_interval` - the interval of timer
// /// * `p_future` - the function which will be executed in each interval
// ///
// /// # Errors
// /// returns Err which comes from callback function
// ///
// /// Example:
// ///
// /// ```no_run
// /// use wolf::chrono::time;
// ///
// /// let res_timer = time::timer(
// ///     Duration::from_secs(1),
// ///     move |p_delta_time_in_secs: f64,
// ///           p_total_time_in_secs: f64,
// ///           p_cancel: &mut bool|
// ///           -> anyhow::Result<f64> {
// ///         println!(
// ///             "timer ticked! delta_time_in_secs is {} total_time_in_secs {}",
// ///             p_delta_time_in_secs, p_total_time_in_secs
// ///         );
// ///         // after 4 seconds, cancel the timer
// ///         if p_total_time_in_secs > 4.0 {
// ///             *p_cancel = true;
// ///         }
// ///         Ok(p_total_time_in_secs)
// ///     },
// /// ).await;
// /// ```
// pub async fn timer<F, R>(p_interval: Duration, mut p_call_back: F) -> anyhow::Result<R>
// where
//     F: FnMut(
//             f64, /* delta time in seconds */
//             f64, /* total time in seconds */
//             &mut bool,
//         ) -> anyhow::Result<R>
//         + Send,
// {
//     let mut interval = tokio::time::interval(p_interval);
//     let mut cancel = false;
//     let base_time = interval.tick().await;
//     let mut last_time = base_time;
//     loop {
//         let current_time = interval.tick().await;
//         let delta_time = current_time - last_time;
//         let total_time = current_time - base_time;
//         last_time = current_time;

//         let ret = p_call_back(
//             delta_time.as_secs_f64(),
//             total_time.as_secs_f64(),
//             &mut cancel,
//         );
//         if cancel {
//             return ret;
//         }
//     }
// }

// /// A timer with interval and callback function
// ///
// /// # Arguments
// ///
// /// * `p_interval` - the interval of timer
// /// * `p_future` - the future which should be executed
// ///
// /// Example:
// ///
// /// ```no_run
// /// use wolf::chrono::time;
// ///
// /// //launch a oneshot timer with 3 seconds
// /// let ret = time::timer_oneshot(Duration::from_secs(3), async {
// ///     println!("oneshot timer ticked! This message comes from the callback function.");
// ///     Ok(())
// /// })
// /// .await;
// /// ```
// pub async fn timer_oneshot<F>(
//     p_interval: Duration,
//     p_future: F,
// ) -> <F as core::future::Future>::Output
// where
//     F: core::future::Future + Send,
// {
//     let mut interval = tokio::time::interval(p_interval);
//     let _r = interval.tick().await;
//     p_future.await
// }

// pub async fn sleep(p_duration: Duration) {
//     tokio::time::sleep(p_duration).await;
// }

// #[derive(Debug, Clone)]
// struct Guard {
//     i: i32,
// }
// impl Drop for Guard {
//     fn drop(&mut self) {
//         println!("Guard dropped");
//     }
// }

// type PIN = Pin<Box<dyn Future<Output = anyhow::Result<()>>>>;

#[test]
fn test() {
    //test timeout for future
    // use crate::wlog;
    // //use std::time::Duration;

    // //let g = Guard { i: 17 };
    // let task1 = || async move {
    //     //wlog!("hello from task1 with {:?}", _g);
    //     std::thread::sleep(Duration::from_secs(3));
    //     wlog!("task1 done");
    //     Ok(()) as anyhow::Result<()>
    // };

    // let mut fut = Box::pin(task1);
    // let task2 = |fut: PIN| async move {
    //     wlog!("hello from task2");
    //     std::thread::sleep(Duration::from_secs(1));
    //     wlog!("task2  done");
    //     Ok(()) as anyhow::Result<()>
    // };

    // futures::executor::block_on(async move {
    //     let _x = futures::join!(task1(), task2(fut));
    // });

    // let ret = timeout(Duration::from_secs(1), fut).await;
    // println!("{:?}", ret);
    // assert!(ret.is_err()); //timeout should return error

    // //launch a oneshot timer with 3 seconds
    // ret = timer_oneshot(Duration::from_secs(3), async {
    //     println!("oneshot timer ticked! The callback function is going to sleep for 1 second.");
    //     sleep(Duration::from_secs(1)).await;
    //     println!("oneshot timer done!");
    //     Ok(())
    // })
    // .await;
    // assert!(ret.is_ok());

    // //launch a timer with 1 seconds interval
    // let res_timer = timer(
    //     Duration::from_secs(1),
    //     move |p_delta_time_in_secs: f64,
    //           p_total_time_in_secs: f64,
    //           p_cancel: &mut bool|
    //           -> anyhow::Result<f64> {
    //         println!(
    //             "timer ticked! delta_time_in_secs is {} total_time_in_secs {}",
    //             p_delta_time_in_secs, p_total_time_in_secs
    //         );

    //         // after 4 seconds, cancel the timer
    //         if p_total_time_in_secs > 4.0 {
    //             *p_cancel = true;
    //         }

    //         Ok(p_total_time_in_secs)
    //     },
    // )
    // .await;
    // assert!(res_timer.is_ok());

    //println!("timer just cancelled after {:?} seconds", res_timer);
}
