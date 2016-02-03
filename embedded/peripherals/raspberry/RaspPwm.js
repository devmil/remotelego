var rpio = require('rpio');
var mapping = require('./RaspPinMap');
var RaspPwm = (function () {
    function RaspPwm(logicalPin) {
        this.mLogicalPin = logicalPin;
        this.mPhysicalPin = mapping.RaspPinMap.getPwm(logicalPin);
    }
    RaspPwm.prototype.open = function (periodUs) {
        this.mPeriodUs = periodUs;
        rpio.open(this.mPhysicalPin, rpio.PWM);
        var tickDurationMS = 1 / (RaspPwm.EFFECTIVE_FREQUENCY_HZ / 1000);
        this.mPeriodTicks = (periodUs * 1000) / tickDurationMS;
        rpio.setRange(this.mPhysicalPin, this.mPeriodTicks);
        rpio.pwmSetClockDivider(128);
    };
    RaspPwm.prototype.setPercentage = function (percentage) {
        var ticks = this.mPeriodTicks * percentage;
        rpio.pwmSetData(this.mPhysicalPin, ticks);
    };
    RaspPwm.BASE_FREQUENCY_HZ = 19200000;
    RaspPwm.CLOCK_DIVIDER = 128;
    RaspPwm.EFFECTIVE_FREQUENCY_HZ = RaspPwm.BASE_FREQUENCY_HZ / RaspPwm.CLOCK_DIVIDER;
    return RaspPwm;
})();
exports.RaspPwm = RaspPwm;
