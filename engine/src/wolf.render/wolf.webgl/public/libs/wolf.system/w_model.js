var __extends = (this && this.__extends) || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
};
var wolf;
(function (wolf) {
    var webgl;
    (function (webgl) {
        var w_model = (function (_super) {
            __extends(w_model, _super);
            function w_model() {
                console.log("w_model");
                _super.call(this);
            }
            w_model.prototype.initialize = function () {
                console.log("run called w_model");
            };
            return w_model;
        }(wolf.system.w_object));
        webgl.w_model = w_model;
    })(webgl = wolf.webgl || (wolf.webgl = {}));
})(wolf || (wolf = {}));
