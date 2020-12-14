namespace wolf {
    export namespace system {
        interface TUpdate {
            (): void;
        }
        export class w_game_time {

            constructor() {
                // TODO: this is test
                console.log("w_game_time");
            }

            // Get elapsed time since the previous Update call.
            public get_elapsed_ticks(): number { return this._elapsed_ticks; };
            public get_elapsed_seconds(): number { return w_game_time.ticks_to_seconds(this._elapsed_ticks); }

            // Get total time since the start of the program.
            public get_total_ticks(): number { return this._total_ticks; }
            public get_total_seconds(): number { return w_game_time.ticks_to_seconds(this._total_ticks); }

            // Get total number of updates since start of the program.
            public get_frame_count(): number { return this._frame_count; }

            // Get the current framerate.
            public get_frames_per_second(): number { return this._fps; }

            //Get is fixed time step
            public get_is_fixed_time_step(): boolean { return this._fixed_time_step; }

            // Set whether to use fixed or variable timestep mode.
            public set_fixed_time_step(pValue: boolean): void { this._fixed_time_step = pValue; }

            // Set how often to call Update when in fixed timestep mode.
            public set_target_elapsed_ticks(pValue: number): void { this._target_elapsed_ticks = pValue; }
            public set_target_elapsed_seconds(pValue: number): void { this._target_elapsed_ticks = w_game_time.seconds_to_ticks(pValue); }

            public static TICKS_PER_SECOND: number = 10000000;
            public static ticks_to_seconds(pTicks: number): number { return pTicks / w_game_time.TICKS_PER_SECOND; }
            public static seconds_to_ticks(pSeconds: number): number { return pSeconds * w_game_time.TICKS_PER_SECOND; }

            //Reset elapsed time
            public reset_elapsed_time(): void {
                this._qp_last_time = Date.now();
                this._left_over_ticks = 0;
                this._fps = 0;
                this._frames_this_second = 0;
                this._qpc_second_counter = 0;
            }

            //Reset elapsed time
            public tick(pUpdate: TUpdate): void {
                let _current_time = Date.now();
                let _time_delta = _current_time - this._qp_last_time;
                this._qp_last_time = _current_time;
                this._qpc_second_counter += (_time_delta / 1000.0);

                let _last_frame_count = this._frame_count;
                if (this._fixed_time_step) {
                    /*
                      If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
                      the clock to exactly match the target value. This prevents tiny and irrelevant errors
                      from accumulating over time. Without this clamping, a game that requested a 60 fps
                      fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
                      accumulate enough tiny errors that it would drop a frame. It is better to just round
                      small deviations down to zero to leave things running smoothly.
                    */
                    if (Math.abs(_time_delta - this._target_elapsed_ticks) < w_game_time.TICKS_PER_SECOND / 4000) {
                        _time_delta = this._target_elapsed_ticks;
                    }
                    this._left_over_ticks += _time_delta;

                    while (this._left_over_ticks >= this._target_elapsed_ticks) {
                        this._elapsed_ticks = this._target_elapsed_ticks;
                        this._total_ticks += this._target_elapsed_ticks;
                        this._left_over_ticks -= this._target_elapsed_ticks;
                        this._frame_count++;

                        pUpdate();
                    }
                }
                else {
                    // Variable timestep update logic.
                    this._elapsed_ticks = _time_delta;
                    this._total_ticks += _time_delta;
                    this._left_over_ticks = 0;
                    this._frame_count++;

                    pUpdate();
                }

                // Track the current framerate.
                if (this._frame_count != _last_frame_count) {
                    this._frames_this_second++;
                }

                if (this._qpc_second_counter >= 1) {
                    this._fps = this._frames_this_second;

                    this._frames_this_second = 0;

                    this._qpc_second_counter %= 1;
                }
            }

            private _name: string = "w_game_time";
            private _qp_frequency: number = 0;
            private _qp_last_time: number = 0;
            // Derived timing data uses a canonical tick format.
            private _elapsed_ticks: number = 0;
            private _total_ticks: number = 0;
            private _left_over_ticks: number = 0;

            // Members for tracking the framerate.
            private _frame_count: number = 0;
            private _fps: number = 0;
            private _frames_this_second: number = 0;
            private _qpc_second_counter: number = 0;

            // Members for configuring fixed timestep mode.
            private _fixed_time_step: boolean = false;
            private _target_elapsed_ticks: number = w_game_time.TICKS_PER_SECOND / 60;
        }
    }
}
