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
        var tickDurationSeconds = 1 / RaspPwm.EFFECTIVE_FREQUENCY_HZ;
        var tickDurationMS = tickDurationSeconds * 1000;
        console.log("PWM Tick duration s = " + tickDurationSeconds);
        console.log("PWM Tick duration ms = " + tickDurationMS);
        this.mPeriodTicks = (periodUs / 1000) / tickDurationMS;
        console.log("PWM Period ticks = " + this.mPeriodTicks);
        rpio.pwmSetRange(this.mPhysicalPin, this.mPeriodTicks);
        rpio.pwmSetClockDivider(RaspPwm.CLOCK_DIVIDER);
    };
    RaspPwm.prototype.setPercentage = function (percentage) {
        var ticks = this.mPeriodTicks * percentage;
        console.log("PWM Data = " + ticks);
        rpio.pwmSetData(this.mPhysicalPin, ticks);
    };
    RaspPwm.BASE_FREQUENCY_HZ = 19200000;
    RaspPwm.CLOCK_DIVIDER = 128;
    RaspPwm.EFFECTIVE_FREQUENCY_HZ = RaspPwm.BASE_FREQUENCY_HZ / RaspPwm.CLOCK_DIVIDER;
    return RaspPwm;
})();
exports.RaspPwm = RaspPwm;
