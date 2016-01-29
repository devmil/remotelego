import edGpio = require('./EdisonGpio');
import edPwm = require('./EdisonPwm');

export class EdisonPeripheralAccess implements IPeripheralAccess {
    getGpio(pin: number) : IGpio {
        return new edGpio.EdisonGpio(pin);
    }
    
    getPwm(pin: number) : IPwm {
        return new edPwm.EdisonPwm(pin);
    } 
}