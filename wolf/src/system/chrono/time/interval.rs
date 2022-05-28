use core::{future::Future, time::Duration};
use instant::{self, Instant};

#[cfg(target_arch = "wasm32")]
use wasm_bindgen::prelude::wasm_bindgen;

#[cfg(not(target_arch = "wasm32"))]
// More accurate time handling by using u64 if possible (native targets)
pub async fn cross_platform_sleep(ms: u64) {
    use tokio::time::{self as tokio_time, Duration as TokioDuration};

    tokio_time::sleep(TokioDuration::from_millis(ms)).await;
}

#[cfg(target_arch = "wasm32")]
#[wasm_bindgen]
pub async fn cross_platform_sleep(ms: u64) {
    use wasm_mt::utils as wasm_mt_utils;

    wasm_mt_utils::sleep(ms as u32).await;
}

pub struct Interval<C, F>
where
    C: Fn(u64) -> F,
    F: Future<Output = ()>,
{
    pub period: Duration,
    // Accepts any kind of Closure or Function Pointer that fits the argument signature so that can be used with non-tokio run-times
    // and maybe a better alternative for the WASM target since it only accepts i32 instead of u64
    pub sleep_handler: C,
    pub started_at: Instant,
    pub ticking_index: u64,
}
impl<C, F> Interval<C, F>
where
    C: Fn(u64) -> F,
    F: Future<Output = ()>,
{
    pub fn new(period: Duration, sleep_handler: C) -> Self {
        Self {
            period,
            sleep_handler,
            started_at: Instant::now(),
            ticking_index: 0,
        }
    }

    pub async fn tick(&mut self) -> Instant {
        let ticking_duration;

        if self.ticking_index > 0 {
            let now = Instant::now();

            ticking_duration = self.started_at + (self.period.mul_f64(self.ticking_index as f64));

            if now.lt(&ticking_duration) {
                let time_difference = ticking_duration - now;

                (self.sleep_handler)(time_difference.as_millis() as u64).await;
            }
        } else {
            ticking_duration = Instant::now();

            self.started_at = ticking_duration;
        }

        self.ticking_index += 1;

        ticking_duration
    }
}
