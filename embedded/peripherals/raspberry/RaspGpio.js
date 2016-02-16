var rpio = require('rpio');
var mapping = require('./RaspPinMap');
var RaspGpio = (function () {
    function RaspGpio(logicalPin) {
        this.mLogicalPin = logicalPin;
        if (logicalPin >= 0) {
            this.mPhysicalPin = mapping.RaspPinMap.getGpio(logicalPin);
        }
    }
    RaspGpio.prototype.open = function (out) {
        if (this.mLogicalPin >= 0) {
            rpio.open(this.mPhysicalPin, out ? rpio.OUTPUT : rpio.INPUT);
        }
    };
    RaspGpio.prototype.write = function (high) {
        if (this.mLogicalPin >= 0) {
            rpio.write(this.mPhysicalPin, high ? rpio.HIGH : rpio.LOW);
        }
    };
    return RaspGpio;
})();
exports.RaspGpio = RaspGpio;
