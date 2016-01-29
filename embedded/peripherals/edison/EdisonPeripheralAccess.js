var edGpio = require('./EdisonGpio');
var edPwm = require('./EdisonPwm');
var EdisonPeripheralAccess = (function () {
    function EdisonPeripheralAccess() {
    }
    EdisonPeripheralAccess.prototype.getGpio = function (pin) {
        return new edGpio.EdisonGpio(pin);
    };
    EdisonPeripheralAccess.prototype.getPwm = function (pin) {
        return new edPwm.EdisonPwm(pin);
    };
    return EdisonPeripheralAccess;
})();
exports.EdisonPeripheralAccess = EdisonPeripheralAccess;
