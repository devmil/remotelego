/**
 * Version 1: hard core values without dimming
 */
var LedControl = (function () {
    function LedControl(peripheralAccess, pin) {
        this.mGpio = peripheralAccess.getGpio(pin);
        this.prepare();
        this.setOnOff(false);
    }
    LedControl.prototype.setOnOff = function (isOn) {
        this.mGpio.write(isOn);
    };
    LedControl.prototype.prepare = function () {
        this.mGpio.open(true);
    };
    return LedControl;
})();
exports.LedControl = LedControl;
