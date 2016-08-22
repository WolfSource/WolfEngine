
require(
    [
        './wolf.system/w_logger',
        './wolf.system/w_game_time',
    ],
    function callback(app) {

        wolf.system.w_logger.user("starting wolf.engine")

        var _game_time = new wolf.system.w_game_time();
        _game_time.set_fixed_time_step(true);
      //  _game_time.set_target_elapsed_seconds(1/60);

        //create main loop
        setTimeout(function run() {

            _game_time.tick(function tick() {

              wolf.system.w_logger.user(_game_time.get_frames_per_second())
              wolf.system.w_logger.user(_game_time.get_elapsed_seconds())

            });

           setTimeout(run, 5);
        }, 5);
    });
