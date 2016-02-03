var rpio = require('rpio');
var mapping = require('./RaspPinMap');
var RaspGpio = (function () {
    function RaspGpio(logicalPin) {
        this.mLogicalPin = logicalPin;
        this.mPhysicalPin = mapping.RaspPinMap.getGpio(logicalPin);
    }
    RaspGpio.prototype.open = function (out) {
        rpio.open(this.mPhysicalPin, out ? rpio.OUTPUT : rpio.INPUT);
    };
    RaspGpio.prototype.write = function (high) {
        rpio.write(this.mPhysicalPin, high ? rpio.HIGH : rpio.LOW);
    };
    return RaspGpio;
})();
exports.RaspGpio = RaspGpio;
