var rgpio = require('./RaspGpio');
var rpwm = require('./RaspPwm');
var RaspPeripheralAccess = (function () {
    function RaspPeripheralAccess() {
    }
    RaspPeripheralAccess.prototype.getGpio = function (pin) {
        return new rgpio.RaspGpio(pin);
    };
    RaspPeripheralAccess.prototype.getPwm = function (pin) {
        return new rpwm.RaspPwm(pin);
    };
    return RaspPeripheralAccess;
})();
exports.RaspPeripheralAccess = RaspPeripheralAccess;
