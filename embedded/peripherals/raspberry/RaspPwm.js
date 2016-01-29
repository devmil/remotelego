var rpio = require('rpio');
var RaspPwm = (function () {
    function RaspPwm(pin) {
        this.mPin = pin;
    }
    // open(range: number) {
    //     rpio.open(this.mPin, rpio.PWM);
    //     rpio.pwmSetRange(this.mPin, range);
    //     //this has to be set after the first pwm has been configured but affects all PWM channels
    //     rpio.pwmSetClockDivider(128);
    // }
    // 
    // setDutyTicks(ticks: number) {
    //     rpio.pwmSetData(this.mPin, ticks);
    // }
    RaspPwm.prototype.open = function (periodUs) {
    };
    RaspPwm.prototype.setPercentage = function (percentage) {
    };
    RaspPwm.prototype.setPulseWidth = function (width) {
    };
    return RaspPwm;
})();
exports.RaspPwm = RaspPwm;
/**
 *         this.mRangeTicks =  this.mEffectiveFrequencyHz / this.mMotorFrequencyHz;

        //control speed
        var pwmDutyTicks: number;
        var directionPinValue : boolean;
        if(percentage < 0) {
            directionPinValue = true;
            //use the other side of the range because we flipped the direction
            pwmDutyTicks = this.mRangeTicks - this.mRangeTicks * (-percentage / 100);
        } else {
            directionPinValue = false;
            pwmDutyTicks = this.mRangeTicks * (percentage / 100);
        }
        console.log('Motor speed: ' + percentage + '%');




        this.mTickDurationMS = 1 / (this.mEffectiveFrequency / 1000);
        //TODO: check that the tick duration is lower than the range, the pulse min and the pulse max
        //calculate the number of timer ticks to achieve the Range
        this.mRangeTicks =  (ServoPWMControl.sRangeMSec / this.mTickDurationMS);
        //calculate the number of timer ticks needed for the lowest position
        this.mDutyLowTicks = this.mRangeTicks * (ServoPWMControl.sPulseMinMSec / ServoPWMControl.sRangeMSec);
        //calculate the number of timer ticks needed for the center position
        this.mDutyCenterTicks = this.mRangeTicks * ((ServoPWMControl.sPulseMaxMSec - ServoPWMControl.sPulseMinMSec) / ServoPWMControl.sRangeMSec);
        //calculate the number of timer ticks needed for the highest position
        this.mDutyHighTicks = this.mRangeTicks * (ServoPWMControl.sPulseMaxMSec / ServoPWMControl.sRangeMSec);

 */ 
