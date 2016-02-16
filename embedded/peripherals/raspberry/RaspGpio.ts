var rpio = require('rpio');
import mapping = require('./RaspPinMap');

export class RaspGpio implements IGpio {
    private mLogicalPin: number;
    private mPhysicalPin: number;
    
    constructor(logicalPin: number) {
        this.mLogicalPin = logicalPin;
        if(logicalPin >= 0) {
            this.mPhysicalPin = mapping.RaspPinMap.getGpio(logicalPin);
        }
    }
    
    open(out: boolean) {
        if(this.mLogicalPin >= 0) {
            rpio.open(this.mPhysicalPin, out ? rpio.OUTPUT : rpio.INPUT);
        }
    }
    
    write(high: boolean) {
        if(this.mLogicalPin >= 0) {
            rpio.write(this.mPhysicalPin, high ? rpio.HIGH : rpio.LOW);
        }
    }
}