var wolf;
(function (wolf) {
    var system;
    (function (system) {
        var w_object = (function () {
            function w_object() {
                console.log("w_object");
            }
            w_object.prototype.run = function () {
                console.log("run called w_object");
            };
            return w_object;
        }());
        system.w_object = w_object;
    })(system = wolf.system || (wolf.system = {}));
})(wolf || (wolf = {}));
