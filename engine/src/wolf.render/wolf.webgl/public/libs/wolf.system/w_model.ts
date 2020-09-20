/// <reference path="./w_object.ts" />
namespace wolf {
    export namespace webgl {
        export class w_model extends wolf.system.w_object {
            constructor() {
                console.log("w_model");
                super();
            }

            public initialize() {
                console.log("run called w_model");
            }
        }
    }
}
