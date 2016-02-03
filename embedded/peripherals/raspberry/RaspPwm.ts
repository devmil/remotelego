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
        var tickDurationMS: number = 1 / (RaspPwm.EFFECTIVE_FREQUENCY_HZ / 1000);
        this.mPeriodTicks = (periodUs * 1000) / tickDurationMS;
        rpio.setRange(this.mPhysicalPin, this.mPeriodTicks);
        rpio.pwmSetClockDivider(128);
    }
    
    setPercentage(percentage: number) {
        var ticks: number = this.mPeriodTicks * percentage;
        rpio.pwmSetData(this.mPhysicalPin, ticks);
    }
}