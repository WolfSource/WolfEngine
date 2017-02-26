namespace wolf {
    export namespace system {
        export class w_logger {
            public static get_date_time(): string {
                return (new Date().toLocaleString());
            }
            public static V(pHresult: boolean, pMSG: string = "Undefined message",
                pTrace: string = "Undefined trace", pLogType: number = 0,
                pTerminateAll: boolean = false) {
                if (pHresult == true) return;
                var _error_msg = "Error on " + pMSG + " with the following error info : " + "Trace info " + pTrace + ".";
                switch (pLogType) {
                    default:
                    case 0://SYSTEM ERROR
                        console.log(_error_msg);
                        break;
                    case 1: //USER
                        console.info(_error_msg);
                        break;
                    case 2: //WARNING:
                        console.warn(_error_msg);
                        break;
                    case 3: //ERROR:
                        console.error(_error_msg);
                        break;
                }
                if (pTerminateAll) {
                    throw new Error("system terminated");
                }
            }
            public static user(pMsg: string) {
                w_logger._write(pMsg, "User");
            }
            public static warning(pMsg: string) {
                w_logger._write(pMsg, "Warning");
            }
            public static error(pMsg: string) {
                w_logger._write(pMsg, "Error");
            }
            private static _write(pMsg: string, pState: string) {
                if (pMsg == null || pMsg == "") {
                    pMsg = "[Wolf] : Info : \tNULL : " + w_logger.get_date_time();
                }
                else {
                    pMsg = "\"" + w_logger.get_date_time() + "\"" +
                        ": {\"msg\":\"" + pMsg + "\",\"state\":\"" + pState + "\"},";
                }
                switch (pState) {
                    default:
                    case "User"://SYSTEM ERROR
                        console.log(pMsg);
                        break;
                    case "Warning": //WARNING:
                        console.warn(pMsg);
                        break;
                    case "Error": //ERROR:
                        console.error(pMsg);
                        break;
                }
            }
        }
    }
}
