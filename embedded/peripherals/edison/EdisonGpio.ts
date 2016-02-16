var mapping = require('./EdisonPinMap');
var mraa = require('mraa');

export class EdisonGpio implements IGpio {
    
    private mMraaPin : number;
    private mLinuxPin: number;
    private mGpio;
    
    constructor(pin: number) {
        this.mLinuxPin = pin;
        if(pin >= 0) {
            this.mMraaPin = mapping.EdisonPinMap.getGpio(pin);
        }
    }
    
    open(out: boolean) {
        if(this.mLinuxPin >= 0) {
            this.mGpio = new mraa.Gpio(this.mMraaPin);
            this.mGpio.dir(out ? mraa.DIR_OUT : mraa.DIR_IN);
        }
    }
    
    write(high: boolean) {
        if(this.mLinuxPin >= 0) {
            this.mGpio.write(high ? 1 : 0);
            console.log('GPIO[' + this.mLinuxPin + ']: ' + high);
        }
    }
}