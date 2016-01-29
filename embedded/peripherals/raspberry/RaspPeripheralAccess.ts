import rgpio = require('./RaspGpio');
import rpwm = require('./RaspPwm');

export class RaspPeripheralAccess implements IPeripheralAccess {
    
    getGpio(pin: number) : IGpio {
        return new rgpio.RaspGpio(pin);
    }
    
    getPwm(pin: number) : IPwm {
        return new rpwm.RaspPwm(pin);
    }   
}