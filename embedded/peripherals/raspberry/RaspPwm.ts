var rpio = require('rpio')
import mapping = require('./RaspPinMap');

export class RaspPwm implements IPwm {
    private static BASE_FREQUENCY_HZ: number = 19200000;
    private static CLOCK_DIVIDER: number = 128;
    private static EFFECTIVE_FREQUENCY_HZ: number = RaspPwm.BASE_FREQUENCY_HZ / RaspPwm.CLOCK_DIVIDER;
    
    private mLogicalPin: number;
    private mPhysicalPin: number;
    
    private mPeriodUs: number;
    private mPeriodTicks: number;
    
    constructor(logicalPin: number) {
        this.mLogicalPin = logicalPin;
        this.mPhysicalPin = mapping.RaspPinMap.getPwm(logicalPin);
    }
    
    open(periodUs: number) {
        this.mPeriodUs = periodUs;
        
        rpio.open(this.mPhysicalPin, rpio.PWM);
        var tickDurationSeconds = 1 / RaspPwm.EFFECTIVE_FREQUENCY_HZ;
        var tickDurationMS: number = tickDurationSeconds * 1000;
        console.log("PWM Tick duration s = " + tickDurationSeconds);
        console.log("PWM Tick duration ms = " + tickDurationMS);
        this.mPeriodTicks = (periodUs / 1000) / tickDurationMS;
        console.log("PWM Period ticks = " + this.mPeriodTicks);
        rpio.pwmSetRange(this.mPhysicalPin, this.mPeriodTicks);
        rpio.pwmSetClockDivider(RaspPwm.CLOCK_DIVIDER);
    }
    
    setPercentage(percentage: number) {
        var ticks: number = this.mPeriodTicks * percentage;
        console.log("PWM Data = " + ticks);
        rpio.pwmSetData(this.mPhysicalPin, ticks);
    }
}