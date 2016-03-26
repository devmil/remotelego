export class ServoPWMControl {
    private mPeripheralAccess: IPeripheralAccess;
    
    private mPin: IPwm;
    private mReverse: boolean;
    
    private static sRangeMSec: number = 20;
    private static sPulseMinMSec: number = 0.5;
    private static sPulseMaxMSec: number = 2.45;
    
    private mAngle: number;
    
    constructor(peripheralAccess: IPeripheralAccess, pin: number, reverseServo: boolean) {
        this.mPeripheralAccess = peripheralAccess;
        this.mPin = peripheralAccess.getPwm(pin);
        this.mReverse = reverseServo;
        this.prepare();
        this.setAngle(0);
    }
    
    private prepare() {
        //configure PWM
        this.mPin.open(ServoPWMControl.sRangeMSec * 1000);
    }
    
    public setAngle(angle: number) {
        //check range -90 <= angle >= 90
        if(angle < -90) {
            angle = -90;
        } else if(angle > 90) {
            angle = 90;
        }

        this.mAngle = angle;
        
        if(this.mReverse) {
            angle *= -1;
        }
        
        var ratio: number = ((angle + 90) / 180);
        var activeMSec = ((ServoPWMControl.sPulseMaxMSec - ServoPWMControl.sPulseMinMSec) * ratio) + ServoPWMControl.sPulseMinMSec;
        var pwmPercentage = activeMSec / ServoPWMControl.sRangeMSec;
        this.mPin.setPercentage(pwmPercentage);

        console.log('Angle = ' + this.mAngle + ', Ratio = ' + ratio + ', Active ms = ' + activeMSec);
    }
    
    public getAngle() : number {
        return this.mAngle;
    }
}