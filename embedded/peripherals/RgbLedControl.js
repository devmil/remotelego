/**
 * first variant: hard r-g-b values without any color calculation and PWM
 */
var RgbLedControl = (function () {
    function RgbLedControl(peripheralAccess, pinRed, pinGreen, pinBlue) {
        this.mRedPin = peripheralAccess.getGpio(pinRed);
        this.mGreenPin = peripheralAccess.getGpio(pinGreen);
        this.mBluePin = peripheralAccess.getGpio(pinBlue);
        this.prepare();
        this.setColor(false, false, false);
    }
    RgbLedControl.prototype.prepare = function () {
        this.mRedPin.open(true);
        this.mGreenPin.open(true);
        this.mBluePin.open(true);
    };
    RgbLedControl.prototype.setColor = function (red, green, blue) {
        this.mRedPin.write(red);
        this.mGreenPin.write(green);
        this.mBluePin.write(blue);
    };
    return RgbLedControl;
})();
exports.RgbLedControl = RgbLedControl;
