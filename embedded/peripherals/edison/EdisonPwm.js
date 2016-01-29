var mapping = require('./EdisonPinMap');
var mraa = require('mraa');
var EdisonPwm = (function () {
    function EdisonPwm(pin) {
        this.mLinuxPin = pin;
        this.mMraaPin = mapping.EdisonPinMap.getPwm(pin);
    }
    EdisonPwm.prototype.open = function (periodUs) {
        this.mPeriondUs = periodUs;
        this.mPwm = new mraa.Pwm(this.mMraaPin);
        this.mPwm.enable(true);
        this.mPwm.period_us(periodUs);
        console.log('PWM[' + this.mLinuxPin + ']: Period = ' + periodUs + " us");
    };
    EdisonPwm.prototype.setPercentage = function (percentage) {
        this.mPwm.write(percentage);
        console.log('PWM[' + this.mLinuxPin + ']: Percentage = ' + (percentage * 100) + '%');
    };
    EdisonPwm.prototype.setPulseWidth = function (width) {
        this.mPwm.enable(true);
        this.mPwm.period_us(this.mPeriondUs);
        this.mPwm.pulsewidth_us(width);
        console.log('PWM[' + this.mLinuxPin + ']: PulseWidth = ' + width + ' us');
    };
    return EdisonPwm;
})();
exports.EdisonPwm = EdisonPwm;
