var mapping = require('./EdisonPinMap');
var mraa = require('mraa');
var EdisonGpio = (function () {
    function EdisonGpio(pin) {
        this.mLinuxPin = pin;
        this.mMraaPin = mapping.EdisonPinMap.getGpio(pin);
    }
    EdisonGpio.prototype.open = function (out) {
        this.mGpio = new mraa.Gpio(this.mMraaPin);
        this.mGpio.dir(out ? mraa.DIR_OUT : mraa.DIR_IN);
    };
    EdisonGpio.prototype.write = function (high) {
        this.mGpio.write(high ? 1 : 0);
        console.log('GPIO[' + this.mLinuxPin + ']: ' + high);
    };
    return EdisonGpio;
})();
exports.EdisonGpio = EdisonGpio;
