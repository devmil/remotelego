var rpio = require('rpio');

export class RaspGpio implements IGpio {
    private mPin: number;
    
    constructor(pin: number) {
        this.mPin = pin;
    }
    
    open(out: boolean) {
        //rpio.open(this.mPin, out ? rpio.OUTPUT : rpio.INPUT);
    }
    
    write(high: boolean) {
        //rpio.write(this.mPin, high ? rpio.HIGH : rpio.LOW);
    }
}