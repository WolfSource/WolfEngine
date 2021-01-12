
require(
    [
        './wolf.system/w_object',
        './wolf.system/w_model',
        './wolf.system/w_logger',
    ],
    function callback(app) {
        var _o = new wolf.webgl.w_model();
        _o.initialize();

        wolf.system.w_logger.user("u")
            //window.document.getElementById();

    });
