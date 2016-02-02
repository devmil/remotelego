var sc = require('../peripherals/ServoPWMControl');
var mc = require('../peripherals/MotorPWMControl');
var led = require('../peripherals/LedControl');
var rgpled = require('../peripherals/RgbLedControl');
var CarState;
(function (CarState) {
    CarState[CarState["Booting"] = 0] = "Booting";
    CarState[CarState["Ready"] = 1] = "Ready";
    CarState[CarState["Connected"] = 2] = "Connected";
})(CarState || (CarState = {}));
var CarModel = (function () {
    function CarModel(peripheralAccess, pinMotorSpeed, pinMotorDirection, pinMotorOnOff, pinServo, motorFrequencyHz, pinFrontLeds, pinBackLeds, pinStateRed, pinStateGreen, pinStateBlue) {
        this.mState = CarState.Booting;
        this.mMotorControl = new mc.MotorPWMControl(peripheralAccess, pinMotorSpeed, pinMotorDirection, pinMotorOnOff, motorFrequencyHz);
        this.mServoControl = new sc.ServoPWMControl(peripheralAccess, pinServo);
        this.mFrontLeds = new led.LedControl(peripheralAccess, pinFrontLeds);
        this.mBackLeds = new led.LedControl(peripheralAccess, pinBackLeds);
        this.mCarStateLed = new rgpled.RgbLedControl(peripheralAccess, pinStateRed, pinStateGreen, pinStateBlue);
        this.setInitialValues();
    }
    CarModel.prototype.setSteering = function (percent) {
        this.mServoControl.setAngle(CarModel.getAngleFromPercent(percent));
    };
    CarModel.prototype.getSteering = function () {
        return CarModel.getPercentFromAngle(this.mServoControl.getAngle());
    };
    CarModel.prototype.setMotorSpeed = function (speedPercentage) {
        this.mMotorControl.setMotorSpeedPercentage(speedPercentage);
    };
    CarModel.prototype.getMotorSpeed = function () {
        return this.mMotorControl.getMotorSpeedPercentage();
    };
    CarModel.prototype.setConnected = function (isConnected) {
        if (isConnected && this.mState == CarState.Ready) {
            this.setState(CarState.Connected);
        }
        else if (!isConnected && this.mState == CarState.Connected) {
            this.setSteering(0);
            this.setMotorSpeed(0);
            this.setState(CarState.Ready);
        }
    };
    CarModel.prototype.visualizeBluetoothClientConnection = function (isConnected) {
        this.setConnected(isConnected);
    };
    //Angle gets inverted because of the way the servo is physically built into the model
    CarModel.getAngleFromPercent = function (percent) {
        return -((percent / 100) * 60); //60 is the max for the lego truck
    };
    CarModel.getPercentFromAngle = function (angle) {
        return -((60 / angle) * 100);
    };
    CarModel.prototype.setInitialValues = function () {
        this.setSteering(0);
        this.setMotorSpeed(0);
        this.mFrontLeds.setOnOff(false);
        this.mBackLeds.setOnOff(false);
        this.setState(CarState.Ready);
    };
    CarModel.prototype.setState = function (state) {
        this.mState = state;
        switch (state) {
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
    };
    return CarModel;
})();
exports.CarModel = CarModel;
