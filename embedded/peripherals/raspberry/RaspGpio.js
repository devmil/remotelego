var rpio = require('rpio');
var RaspGpio = (function () {
    function RaspGpio(pin) {
        this.mPin = pin;
    }
    RaspGpio.prototype.open = function (out) {
        //rpio.open(this.mPin, out ? rpio.OUTPUT : rpio.INPUT);
    };
    RaspGpio.prototype.write = function (high) {
        //rpio.write(this.mPin, high ? rpio.HIGH : rpio.LOW);
    };
    return RaspGpio;
})();
exports.RaspGpio = RaspGpio;
