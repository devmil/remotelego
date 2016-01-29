var mapping = require('./EdisonPinMap')
var mraa = require('mraa');

export class EdisonPwm implements IPwm {
    
    private mLinuxPin: number;
    private mMraaPin: number;
    private mPeriondUs: number;
    private mPwm;
    
    constructor(pin: number) {
        this.mLinuxPin = pin;
        this.mMraaPin = mapping.EdisonPinMap.getPwm(pin);
    }
    
    open(periodUs: number) {
        this.mPeriondUs = periodUs;
        this.mPwm = new mraa.Pwm(this.mMraaPin);
        this.mPwm.enable(true);
        this.mPwm.period_us(periodUs);
        console.log('PWM[' + this.mLinuxPin + ']: Period = ' + periodUs + " us");
    }
    
    setPercentage(percentage: number) {
        this.mPwm.write(percentage);
        console.log('PWM[' + this.mLinuxPin + ']: Percentage = ' + (percentage * 100) + '%');
    }
    
    setPulseWidth(width: number) {
        this.mPwm.enable(true);
        this.mPwm.period_us(this.mPeriondUs);
        this.mPwm.pulsewidth_us(width);
        console.log('PWM[' + this.mLinuxPin + ']: PulseWidth = ' + width + ' us');
    }

}