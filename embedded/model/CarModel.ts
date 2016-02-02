import sc = require('../peripherals/ServoPWMControl');
import mc = require('../peripherals/MotorPWMControl');
import led = require('../peripherals/LedControl');
import rgpled = require('../peripherals/RgbLedControl')

enum CarState {
    Booting,
    Ready,
    Connected    
}


export class CarModel implements ICarModel, IBluetoothStateVisualizer {
    private mMotorControl: mc.MotorPWMControl;
    private mServoControl: sc.ServoPWMControl;
    private mFrontLeds: led.LedControl;
    private mBackLeds: led.LedControl;
    private mCarStateLed: rgpled.RgbLedControl;
    
    private mState: CarState;
    
    constructor(
        peripheralAccess: IPeripheralAccess, 
        pinMotorSpeed: number, 
        pinMotorDirection: number, 
        pinMotorOnOff: number, 
        pinServo: number,
        motorFrequencyHz: number,
        pinFrontLeds: number,
        pinBackLeds: number,
        pinStateRed: number,
        pinStateGreen: number,
        pinStateBlue: number) {
            
        this.mState = CarState.Booting;
            
        this.mMotorControl = new mc.MotorPWMControl(
                                peripheralAccess, 
                                pinMotorSpeed, 
                                pinMotorDirection, 
                                pinMotorOnOff, 
                                motorFrequencyHz)
        this.mServoControl = new sc.ServoPWMControl(
                                    peripheralAccess, 
                                    pinServo);
                                    
        this.mFrontLeds = new led.LedControl(peripheralAccess, pinFrontLeds);
        this.mBackLeds = new led.LedControl(peripheralAccess, pinBackLeds);
        this.mCarStateLed = new rgpled.RgbLedControl(peripheralAccess, pinStateRed, pinStateGreen, pinStateBlue);
                              
                              
        this.setInitialValues();      
    }
    
    public setSteering(percent: number) {
        this.mServoControl.setAngle(CarModel.getAngleFromPercent(percent));
    }
    
    public getSteering(): number {
        return CarModel.getPercentFromAngle(this.mServoControl.getAngle());
    }
    
    public setMotorSpeed(speedPercentage: number) {
        this.mMotorControl.setMotorSpeedPercentage(speedPercentage);
    }
    
    public getMotorSpeed(): number {
        return this.mMotorControl.getMotorSpeedPercentage();
    }
    
    public setConnected(isConnected: boolean) {
        if(isConnected && this.mState == CarState.Ready) {
            this.setState(CarState.Connected);
        } else if(!isConnected && this.mState == CarState.Connected) {
            this.setSteering(0);
            this.setMotorSpeed(0);
            this.setState(CarState.Ready);
        }
    }
    
    public visualizeBluetoothClientConnection(isConnected: boolean) {
        this.setConnected(isConnected);
    }
    
    //Angle gets inverted because of the way the servo is physically built into the model
    private static getAngleFromPercent(percent: number): number {
        return -((percent/100) * 60); //60 is the max for the lego truck
    }
    
    private static getPercentFromAngle(angle: number): number {
        return -((60/angle) * 100);
    }
    
    private setInitialValues() {
        this.setSteering(0);
        this.setMotorSpeed(0);
        this.mFrontLeds.setOnOff(false);
        this.mBackLeds.setOnOff(false);
        this.setState(CarState.Ready);        
    }
    
    private setState(state: CarState) {
        this.mState = state;
        switch(state) {
            case CarState.Booting:
                this.mCarStateLed.setColor(true, false, false);
                this.mFrontLeds.setOnOff(false);
                this.mBackLeds.setOnOff(false);
                break;
            case CarState.Ready:
                this.mCarStateLed.setColor(false, true, false);
                this.mFrontLeds.setOnOff(false);
                this.mBackLeds.setOnOff(false);
                break;
            case CarState.Connected:
                this.mCarStateLed.setColor(false, false, true);
                this.mFrontLeds.setOnOff(true);
                this.mBackLeds.setOnOff(true);
                break;
        }
    }
}