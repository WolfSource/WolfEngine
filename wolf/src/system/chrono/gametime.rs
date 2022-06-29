use instant::Instant;

// Note that even if the wasm-bindgen feature is enabled,
// this crate will continue to rely on std::time::Instant as long as you are not targeting wasm32.
// This allows for portable code that will work on both native and WASM platforms.

//milliseconds per second
const MAX_DELTA_IN_MILLIS: f64 = 60_000.0;

/// A measurement of the game time based on high resolution clock.
///
/// Example:
///
/// ```no_run
/// use wolf::chrono::gametime::GameTime;
/// let mut gtime = GameTime::new();
/// gtime.set_fixed_time_step(true);
/// gtime.set_target_elapsed_seconds(1 / 60.0); //ticks every 16.666 ms (60 fps)
///
/// loop {
///     gtime.tick_fn(move || {
///         //tick every 16.666 ms
///     });
/// }
/// ```
#[derive(Debug, Clone, Copy)]
pub struct GameTime {
    pub last_time: Instant,
    pub elapsed_secs: f64,
    pub total_secs: f64,
    pub left_over_ticks: f64,
    pub frame_count: u32,
    pub fps: u32,
    pub frames_this_sec: u32,
    pub seconds_counter: f64,
    pub fixed_time_step: bool,
    pub target_elapsed_secs: f64,
}

impl Default for GameTime {
    fn default() -> Self {
        Self {
            last_time: Instant::now(),
            elapsed_secs: 0.0,
            total_secs: 0.0,
            left_over_ticks: 0.0,
            frame_count: 0,
            fps: 0,
            frames_this_sec: 0,
            seconds_counter: 0.0,
            fixed_time_step: false,
            target_elapsed_secs: 1.0 / 60.0,
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
        self.frames_this_sec = 0;
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
    pub const fn get_elapsed_seconds(&self) -> f64 {
        self.elapsed_secs
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
    pub const fn get_total_elapsed_seconds(&self) -> f64 {
        self.total_secs
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
        self.target_elapsed_secs = p_value;
    }

    /// the update tick function
    ///
    /// # Arguments
    ///
    /// * `p_on_tick_call_back` - the async function callback which will be raised in each tick
    ///
    /// Example:
    ///
    /// ```
    /// use wolf::chrono::gametime::GameTime;
    ///
    /// let mut gtime = GameTime::new();
    ///
    /// gtime.set_target_elapsed_seconds(1.0 / 60.0);
    /// gtime.tick();
    /// ```
    #[allow(unused_must_use)]
    pub fn tick(&mut self) {
        self.tick_fn(move || {});
    }

    /// the update tick function
    ///
    /// # Arguments
    ///
    /// * `p_on_tick_call_back` - the async function callback which will be raised in each tick
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
    pub fn tick_fn(&mut self, mut p_on_tick_fn: impl FnMut()) {
        // Query the current time.
        let current_time = Instant::now();
        let mut time_delta = (current_time - self.last_time).as_secs_f64();

        self.last_time = current_time;
        self.seconds_counter += time_delta;

        //clamp excessively large time deltas (e.g. after paused in the debugger).
        if time_delta > MAX_DELTA_IN_MILLIS {
            time_delta = MAX_DELTA_IN_MILLIS;
        }

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
            let abs_ticks = f64::abs(time_delta - self.target_elapsed_secs);
            if abs_ticks < 0.0004 {
                time_delta = self.target_elapsed_secs;
            }

            self.left_over_ticks += time_delta;
            while self.left_over_ticks >= self.target_elapsed_secs {
                self.elapsed_secs = self.target_elapsed_secs;
                self.total_secs += self.target_elapsed_secs;
                self.left_over_ticks -= self.target_elapsed_secs;
                self.frame_count += 1;

                p_on_tick_fn();
            }
        } else {
            // Variable timestep update logic.
            self.elapsed_secs = time_delta;
            self.total_secs += time_delta;
            self.left_over_ticks = 0.0;
            self.frame_count += 1;

            p_on_tick_fn();
        }

        // Track the current framerate.
        self.frames_this_sec += self.frame_count - last_frame_count;

        if self.seconds_counter >= 1.0 {
            self.fps = self.frames_this_sec;
            self.frames_this_sec = 0;
            self.seconds_counter %= 1.0;
        }
    }
}
