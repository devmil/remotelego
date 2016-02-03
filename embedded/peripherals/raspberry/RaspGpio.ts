var rpio = require('rpio');
import mapping = require('./RaspPinMap');

export class RaspGpio implements IGpio {
    private mLogicalPin: number;
    private mPhysicalPin: number;
    
    constructor(logicalPin: number) {
        this.mLogicalPin = logicalPin;
        this.mPhysicalPin = mapping.RaspPinMap.getGpio(logicalPin);
    }
    
    open(out: boolean) {
        rpio.open(this.mPhysicalPin, out ? rpio.OUTPUT : rpio.INPUT);
    }
    
    write(high: boolean) {
        rpio.write(this.mPhysicalPin, high ? rpio.HIGH : rpio.LOW);
    }
}