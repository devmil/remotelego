/**
 * first variant: hard r-g-b values without any color calculation and PWM
 */
export class RgbLedControl {
    private mRedPin: IGpio;
    private mGreenPin: IGpio;
    private mBluePin: IGpio;
    
    constructor(peripheralAccess: IPeripheralAccess, pinRed: number, pinGreen: number, pinBlue: number) {
        this.mRedPin = peripheralAccess.getGpio(pinRed);
        this.mGreenPin = peripheralAccess.getGpio(pinGreen);
        this.mBluePin = peripheralAccess.getGpio(pinBlue);
        
        this.prepare();
        
        this.setColor(false, false, false);
    }
    
    private prepare() {
        this.mRedPin.open(true);
        this.mGreenPin.open(true);
        this.mBluePin.open(true);
    }
    
    public setColor(red: boolean, green: boolean, blue: boolean) {
        this.mRedPin.write(red);
        this.mGreenPin.write(green);
        this.mBluePin.write(blue);
    }
}