/**
 * Version 1: hard core values without dimming
 */
export class LedControl {
    private mGpio: IGpio;
    
    constructor(peripheralAccess: IPeripheralAccess, pin: number) {
        this.mGpio = peripheralAccess.getGpio(pin);
        
        this.prepare();
        
        this.setOnOff(false);
    }
    
    public setOnOff(isOn: boolean) {
        this.mGpio.write(isOn);
    }
    
    prepare() {
        this.mGpio.open(true);
    }
}