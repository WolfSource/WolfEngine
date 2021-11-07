use std::time::Instant;

//integer format represents time using 10,000,000 ticks per second
const TICKS_PER_SECOND: f64 = 10000000.0;

#[inline]
fn ticks_to_seconds(p_ticks: f64) -> f64 {
    p_ticks / TICKS_PER_SECOND
}

#[inline]
fn seconds_to_ticks(p_seconds: f64) -> f64 {
    p_seconds * TICKS_PER_SECOND
}

//the structure of GameTime
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
            max_delta: 313918.0,
            elapsed_ticks: 0.0,
            total_ticks: 0.0,
            left_over_ticks: 0.0,
            frame_count: 0,
            fps: 0,
            frames_this_second: 0,
            seconds_counter: 0.0,
            fixed_time_step: false,
            target_elapsed_ticks: (TICKS_PER_SECOND as f64) / (60.0 as f64),
        }
    }
}

// Implementation block, all `GameTime` methods go in here
impl GameTime {
    /**
     * initialize GameTime object
     */
    pub fn new() -> Self {
        let gt: GameTime = Default::default();
        gt
    }

    /**
     * reset the GameTime
     */
    pub fn reset(&mut self) {
        //Get current time in seconds
        self.last_time = Instant::now();
        self.left_over_ticks = 0.0;
        self.fps = 0;
        self.frames_this_second = 0;
        self.seconds_counter = 0.0;
    }

    /**
     * set fixed time step mode
     * @return elapsed total ticks
     */
    pub fn set_fixed_time_step(&mut self, p_value: bool) -> () {
        self.fixed_time_step = p_value;
    }

    /**
     * set fixed time step mode
     * @return elapsed total ticks
     */
    pub fn get_fixed_time_step(&self) -> bool {
        self.fixed_time_step
    }

    /**
     * get elapsed seconds since the previous tick call.
     * @return elapsed time
     */
    pub fn get_elapsed_seconds(&self) -> f64 {
        ticks_to_seconds(self.elapsed_ticks)
    }

    /**
     * get total ticks since the start.
     * @return elapsed total ticks
     */
    pub fn get_total_elapsed_seconds(&self) -> f64 {
        ticks_to_seconds(self.total_ticks)
    }

    /**
     * set target elapsed seconds.
     * @return elapsed total ticks
     */
    pub fn set_target_elapsed_seconds(&mut self, p_value: f64) -> () {
        self.target_elapsed_ticks = seconds_to_ticks(p_value);
    }

    /**
     * get total ticks since the start
     */
    pub fn tick_fn(&mut self, mut p_on_tick_call_back: impl FnMut() -> ()) -> () {
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
        time_delta = time_delta * (TICKS_PER_SECOND as f64);

        let _last_frame_count = self.frame_count;
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
                time_delta = self.target_elapsed_ticks as f64;
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
        self.frames_this_second += self.frame_count - _last_frame_count;

        if self.seconds_counter >= 1.0 {
            self.fps = self.frames_this_second;
            self.frames_this_second = 0;
            self.seconds_counter %= 1.0;
        }
    }

    pub fn tick(&mut self) -> () {
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
        time_delta = time_delta * (TICKS_PER_SECOND as f64);

        let _last_frame_count = self.frame_count;
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
                time_delta = self.target_elapsed_ticks as f64;
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
        self.frames_this_second += self.frame_count - _last_frame_count;

        if self.seconds_counter >= 1.0 {
            self.fps = self.frames_this_second;
            self.frames_this_second = 0;
            self.seconds_counter %= 1.0;
        }
    }
}

#[test]
fn test() {
    let mut g = GameTime::new();
    g.set_fixed_time_step(true);
    g.set_target_elapsed_seconds(1.0 / 60.0); //ticks every 0.016ms (60 fps)

    loop {
        g.tick_fn(move || -> () {
            println!(
                "elpased seconds from last tick {}. total elapsed seconds {}",
                g.get_elapsed_seconds(),
                g.get_total_elapsed_seconds(),
            );
        });

        // break after 10 seconds
        if g.get_total_elapsed_seconds() > 10.0 {
            break;
        }
    }
}
