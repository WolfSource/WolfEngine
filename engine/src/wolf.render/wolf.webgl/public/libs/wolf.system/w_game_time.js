var wolf;
(function (wolf) {
    var system;
    (function (system) {
        var w_game_time = (function () {
            function w_game_time() {
                this._name = "w_game_time";
                this._qp_frequency = 0;
                this._qp_last_time = 0;
                this._elapsed_ticks = 0;
                this._total_ticks = 0;
                this._left_over_ticks = 0;
                this._frame_count = 0;
                this._fps = 0;
                this._frames_this_second = 0;
                this._qpc_second_counter = 0;
                this._fixed_time_step = false;
                this._target_elapsed_ticks = w_game_time.TICKS_PER_SECOND / 60;
                console.log("w_game_time");
            }
            w_game_time.prototype.get_elapsed_ticks = function () { return this._elapsed_ticks; };
            ;
            w_game_time.prototype.get_elapsed_seconds = function () { return w_game_time.ticks_to_seconds(this._elapsed_ticks); };
            w_game_time.prototype.get_total_ticks = function () { return this._total_ticks; };
            w_game_time.prototype.get_total_seconds = function () { return w_game_time.ticks_to_seconds(this._total_ticks); };
            w_game_time.prototype.get_frame_count = function () { return this._frame_count; };
            w_game_time.prototype.get_frames_per_second = function () { return this._fps; };
            w_game_time.prototype.get_is_fixed_time_step = function () { return this._fixed_time_step; };
            w_game_time.prototype.set_fixed_time_step = function (pValue) { this._fixed_time_step = pValue; };
            w_game_time.prototype.set_target_elapsed_ticks = function (pValue) { this._target_elapsed_ticks = pValue; };
            w_game_time.prototype.set_target_elapsed_seconds = function (pValue) { this._target_elapsed_ticks = w_game_time.seconds_to_ticks(pValue); };
            w_game_time.ticks_to_seconds = function (pTicks) { return pTicks / w_game_time.TICKS_PER_SECOND; };
            w_game_time.seconds_to_ticks = function (pSeconds) { return pSeconds * w_game_time.TICKS_PER_SECOND; };
            w_game_time.prototype.reset_elapsed_time = function () {
                this._qp_last_time = Date.now();
                this._left_over_ticks = 0;
                this._fps = 0;
                this._frames_this_second = 0;
                this._qpc_second_counter = 0;
            };
            w_game_time.prototype.tick = function (pUpdate) {
                var _current_time = Date.now();
                var _time_delta = _current_time - this._qp_last_time;
                this._qp_last_time = _current_time;
                this._qpc_second_counter += (_time_delta / 1000.0);
                var _last_frame_count = this._frame_count;
                if (this._fixed_time_step) {
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
                    this._elapsed_ticks = _time_delta;
                    this._total_ticks += _time_delta;
                    this._left_over_ticks = 0;
                    this._frame_count++;
                    pUpdate();
                }
                if (this._frame_count != _last_frame_count) {
                    this._frames_this_second++;
                }
                if (this._qpc_second_counter >= 1) {
                    this._fps = this._frames_this_second;
                    this._frames_this_second = 0;
                    this._qpc_second_counter %= 1;
                }
            };
            w_game_time.TICKS_PER_SECOND = 10000000;
            return w_game_time;
        }());
        system.w_game_time = w_game_time;
    })(system = wolf.system || (wolf.system = {}));
})(wolf || (wolf = {}));
