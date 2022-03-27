// Note that even if the wasm-bindgen feature is enabled,
// this crate will continue to rely on std::time::Instant as long as you are not targeting wasm32.
// This allows for portable code that will work on both native and WASM platforms.
use instant::Instant;

//integer format represents time using 10,000,000 ticks per second
const TICKS_PER_SECOND: f64 = 10_000_000.0;

#[inline]
fn ticks_to_seconds(p_ticks: f64) -> f64 {
    p_ticks / TICKS_PER_SECOND
}

#[inline]
fn seconds_to_ticks(p_seconds: f64) -> f64 {
    p_seconds * TICKS_PER_SECOND
}

/// A measurement of the game time based on system clock.
///
/// Example:
///
/// ```no_run
/// use wolf::chrono::gametime::GameTime;
/// let mut gtime = GameTime::new();
/// gtime.set_fixed_time_step(true);
/// gtime.set_target_elapsed_seconds(1.0 / 60.0); //ticks every 0.016 sec (60 fps)
///
/// loop {
///     gtime.tick_fn(move || {
///         //tick every 16ms
///     });
/// }
/// ```
#[derive(Debug, Clone, Copy)]
pub struct GameTime {
    pub last_time: Instant,
    pub max_delta: f64,
    pub elapsed_ticks: f64,
    pub total_ticks: f64,
    pub left_over_ticks: f64,
    pub frame_count: u32,
    pub fps: u32,
    pub frames_this_second: u32,
    pub seconds_counter: f64,
    pub fixed_time_step: bool,
    pub target_elapsed_ticks: f64,
}

impl Default for GameTime {
    fn default() -> Self {
        Self {
            last_time: Instant::now(),
            max_delta: 313_918.0,
            elapsed_ticks: 0.0,
            total_ticks: 0.0,
            left_over_ticks: 0.0,
            frame_count: 0,
            fps: 0,
            frames_this_second: 0,
            seconds_counter: 0.0,
            fixed_time_step: false,
            target_elapsed_ticks: TICKS_PER_SECOND / 60.0,
        }
    }
}

// Implementation block, all `GameTime` methods go in here
impl GameTime {
    /// initialize and returns a `GameTime` instance
    ///
    /// Example:
    ///
    /// ```
    /// use wolf::chrono::gametime::GameTime;
    ///
    /// let gtime = GameTime::new();
    /// ```
    #[must_use]
    pub fn new() -> Self {
        Self::default()
    }

    /// reset the `GameTime` instance
    ///
    /// Example:
    ///
    /// ```
    /// use wolf::chrono::gametime::GameTime;
    ///
    /// let mut gtime = GameTime::new();
    /// gtime.reset();
    /// ```
    pub fn reset(&mut self) {
        //Get current time in seconds
        self.last_time = Instant::now();
        self.left_over_ticks = 0.0;
        self.fps = 0;
        self.frames_this_second = 0;
        self.seconds_counter = 0.0;
    }

    /// set fixed time step mode for the `GameTime` instance
    ///
    /// # Arguments
    ///
    /// * `p_value` - true means game time will be called in specific interval. Best option for matching with GPU's `VSync`
    ///
    /// Example:
    ///
    /// ```
    /// use wolf::chrono::gametime::GameTime;
    ///
    /// let mut gtime = GameTime::new();
    /// gtime.set_fixed_time_step(true);//fixed time step is enable
    /// gtime.tick_fn(|| { /* tick on certain interval */ });
    /// ```
    pub fn set_fixed_time_step(&mut self, p_value: bool) {
        self.fixed_time_step = p_value;
    }

    /// get the state of fixed time step mode
    ///
    /// Example:
    ///
    /// ```
    /// use wolf::chrono::gametime::GameTime;
    ///
    /// let mut gtime = GameTime::new();
    /// let is_enable = gtime.get_fixed_time_step();
    /// ```
    #[must_use]
    pub const fn get_fixed_time_step(&self) -> bool {
        self.fixed_time_step
    }

    #[must_use]
    /// returns the elapsed seconds since the previous tick call.
    ///
    /// Example:
    ///
    /// ```
    /// use wolf::chrono::gametime::GameTime;
    ///
    /// let mut gtime = GameTime::new();
    /// let elapsed_time = gtime.get_elapsed_seconds();
    /// ```
    pub fn get_elapsed_seconds(&self) -> f64 {
        ticks_to_seconds(self.elapsed_ticks)
    }

    /// returns the total ticks since the start.
    ///
    /// Example:
    ///
    /// ```
    /// use wolf::chrono::gametime::GameTime;
    ///
    /// let gtime = GameTime::new();
    /// let total_time = gtime.get_total_elapsed_seconds();
    /// ```
    #[must_use]
    pub fn get_total_elapsed_seconds(&self) -> f64 {
        ticks_to_seconds(self.total_ticks)
    }

    /// set target elapsed seconds.
    ///
    /// # Arguments
    ///
    /// * `p_value` - the value of target elapsed seconds
    ///
    /// Example:
    ///
    /// ```
    /// use wolf::chrono::gametime::GameTime;
    ///
    /// let mut gtime = GameTime::new();
    /// gtime.set_target_elapsed_seconds(1.0 / 60.0); //ticks every 0.016 sec (60 fps)
    /// ```
    pub fn set_target_elapsed_seconds(&mut self, p_value: f64) {
        self.target_elapsed_ticks = seconds_to_ticks(p_value);
    }

    /// the update tick function
    ///
    /// # Arguments
    ///
    /// * `p_on_tick_call_back` - the function callback which will be raised in each tick
    ///
    /// Example:
    ///
    /// ```
    /// use wolf::chrono::gametime::GameTime;
    ///
    /// let mut gtime = GameTime::new();
    ///
    /// gtime.set_target_elapsed_seconds(1.0 / 60.0);
    /// gtime.tick_fn(move || {
    ///     //this callback function will call every 16 ms
    /// });
    /// ```
    pub fn tick_fn(&mut self, mut p_on_tick_call_back: impl FnMut()) {
        // Query the current time.
        let current_time = Instant::now();
        let mut time_delta = (current_time - self.last_time).as_secs_f64();

        self.last_time = current_time;
        self.seconds_counter += time_delta;

        //clamp excessively large time deltas (e.g. after paused in the debugger).
        if time_delta > self.max_delta {
            time_delta = self.max_delta;
        }

        // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
        time_delta *= TICKS_PER_SECOND;

        let last_frame_count = self.frame_count;
        if self.fixed_time_step {
            /*
                If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
                the clock to exactly match the target value. This prevents tiny and irrelevant errors
                from accumulating over time. Without this clamping, a game that requested a 60 fps
                fixed update, running with VSync enabled on a 59.94 NTSC display, would eventually
                accumulate enough tiny errors that it would drop a frame. It is better to just round
                small deviations down to zero to leave things running smoothly.
            */
            let abs_ticks = f64::abs(time_delta - self.target_elapsed_ticks);
            let ticks_per_sec_4 = TICKS_PER_SECOND / 4000.0;

            if abs_ticks < ticks_per_sec_4 {
                time_delta = self.target_elapsed_ticks;
            }

            self.left_over_ticks += time_delta;
            while self.left_over_ticks >= self.target_elapsed_ticks {
                self.elapsed_ticks = self.target_elapsed_ticks;
                self.total_ticks += self.target_elapsed_ticks;
                self.left_over_ticks -= self.target_elapsed_ticks;
                self.frame_count += 1;

                p_on_tick_call_back();
            }
        } else {
            // Variable timestep update logic.
            self.elapsed_ticks = time_delta;
            self.total_ticks += time_delta;
            self.left_over_ticks = 0.0;
            self.frame_count += 1;

            p_on_tick_call_back();
        }

        // Track the current framerate.
        self.frames_this_second += self.frame_count - last_frame_count;

        if self.seconds_counter >= 1.0 {
            self.fps = self.frames_this_second;
            self.frames_this_second = 0;
            self.seconds_counter %= 1.0;
        }
    }

    /// the update tick function
    ///
    /// Example:
    ///
    /// ```
    /// use wolf::chrono::gametime::GameTime;
    ///
    /// let mut gtime = GameTime::new();
    /// gtime.set_target_elapsed_seconds(1.0 / 60.0);
    /// gtime.tick();// a simple tick with no callback
    /// ```
    pub fn tick(&mut self) {
        // Query the current time.
        let current_time = Instant::now();
        let mut time_delta = (current_time - self.last_time).as_secs_f64();

        self.last_time = current_time;
        self.seconds_counter += time_delta;

        //clamp excessively large time deltas (e.g. after paused in the debugger).
        if time_delta > self.max_delta {
            time_delta = self.max_delta;
        }

        // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
        time_delta *= TICKS_PER_SECOND;

        let last_frame_count = self.frame_count;
        if self.fixed_time_step {
            /*
                If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
                the clock to exactly match the target value. This prevents tiny and irrelevant errors
                from accumulating over time. Without this clamping, a game that requested a 60 fps
                fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
                accumulate enough tiny errors that it would drop a frame. It is better to just round
                small deviations down to zero to leave things running smoothly.
            */
            let abs_ticks = f64::abs(time_delta - self.target_elapsed_ticks);
            let ticks_per_sec_4 = TICKS_PER_SECOND / 4000.0;

            if abs_ticks < ticks_per_sec_4 {
                time_delta = self.target_elapsed_ticks;
            }

            self.left_over_ticks += time_delta;
            while self.left_over_ticks >= self.target_elapsed_ticks {
                self.elapsed_ticks = self.target_elapsed_ticks;
                self.total_ticks += self.target_elapsed_ticks;
                self.left_over_ticks -= self.target_elapsed_ticks;
                self.frame_count += 1;
            }
        } else {
            // Variable timestep update logic.
            self.elapsed_ticks = time_delta;
            self.total_ticks += time_delta;
            self.left_over_ticks = 0.0;
            self.frame_count += 1;
        }

        // Track the current framerate.
        self.frames_this_second += self.frame_count - last_frame_count;

        if self.seconds_counter >= 1.0 {
            self.fps = self.frames_this_second;
            self.frames_this_second = 0;
            self.seconds_counter %= 1.0;
        }
    }
}

#[test]
fn test() {
    use super::gametime::GameTime;
    let mut gtime = GameTime::new();
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
