var wolf;
(function (wolf) {
    var system;
    (function (system) {
        var w_logger = (function () {
            function w_logger() {
            }
            w_logger.get_date_time = function () {
                return (new Date().toLocaleString());
            };
            w_logger.V = function (pHresult, pMSG, pTrace, pLogType, pTerminateAll) {
                if (pMSG === void 0) { pMSG = "Undefined message"; }
                if (pTrace === void 0) { pTrace = "Undefined trace"; }
                if (pLogType === void 0) { pLogType = 0; }
                if (pTerminateAll === void 0) { pTerminateAll = false; }
                if (pHresult == true)
                    return;
                var _error_msg = "Error on " + pMSG + " with the following error info : " + "Trace info " + pTrace + ".";
                switch (pLogType) {
                    default:
                    case 0:
                        console.log(_error_msg);
                        break;
                    case 1:
                        console.info(_error_msg);
                        break;
                    case 2:
                        console.warn(_error_msg);
                        break;
                    case 3:
                        console.error(_error_msg);
                        break;
                }
                if (pTerminateAll) {
                    throw new Error("system terminated");
                }
            };
            w_logger.user = function (pMsg) {
                w_logger._write(pMsg, "User");
            };
            w_logger.warning = function (pMsg) {
                w_logger._write(pMsg, "Warning");
            };
            w_logger.error = function (pMsg) {
                w_logger._write(pMsg, "Error");
            };
            w_logger._write = function (pMsg, pState) {
                if (pMsg == null || pMsg == "") {
                    pMsg = "[Wolf] : Info : \tNULL : " + w_logger.get_date_time();
                }
                else {
                    pMsg = "\"" + w_logger.get_date_time() + "\"" +
                        ": {\"msg\":\"" + pMsg + "\",\"state\":\"" + pState + "\"},";
                }
                switch (pState) {
                    default:
                    case "User":
                        console.log(pMsg);
                        break;
                    case "Warning":
                        console.warn(pMsg);
                        break;
                    case "Error":
                        console.error(pMsg);
                        break;
                }
            };
            return w_logger;
        }());
        system.w_logger = w_logger;
    })(system = wolf.system || (wolf.system = {}));
})(wolf || (wolf = {}));
