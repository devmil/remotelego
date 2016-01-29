var MotorPWMControl = (function () {
    function MotorPWMControl(peripheralAccess, pinSpeedControl, pinDirection, pinOnOff, motorFrequencyHz) {
        this.mPeripheralAccess = peripheralAccess;
        this.mPinSpeedControl = peripheralAccess.getPwm(pinSpeedControl);
        this.mPinDirectionControl = peripheralAccess.getGpio(pinDirection);
        this.mPinOnOffControl = peripheralAccess.getGpio(pinOnOff);
        this.mMotorFrequencyHz = motorFrequencyHz;
        this.prepare();
        //TODO: check that motor frequency is lower than the effective frequency
        this.setMotorSpeedPercentage(0);
    }
    MotorPWMControl.prototype.prepare = function () {
        //configure PWM
        var periodSec = 1 / this.mMotorFrequencyHz;
        this.mPinSpeedControl.open(periodSec * 1000 * 1000);
        this.mPinDirectionControl.open(true);
        this.mPinOnOffControl.open(true);
    };
    MotorPWMControl.prototype.setMotorSpeedPercentage = function (percentage) {
        //make sure that percentage is between -100 and 100
        if (percentage < -100) {
            percentage = -100;
        }
        else if (percentage > 100) {
            percentage = 100;
        }
        this.mMotorPercentage = percentage;
        console.log('Motor speed: ' + percentage + '%');
        //control speed
        var directionPinValue;
        if (percentage < 0) {
            directionPinValue = true;
            percentage = 100 + percentage;
        }
        else {
            directionPinValue = false;
        }
        //control On/Off
        if (this.mMotorPercentage == 0) {
            this.mPinOnOffControl.write(false);
            console.log('On/Off = Off');
        }
        else {
            this.mPinOnOffControl.write(true);
            console.log('On/Off = On');
        }
        //control PWM
        this.mPinSpeedControl.setPercentage(percentage / 100);
        //control direction
        this.mPinDirectionControl.write(directionPinValue);
    };
    MotorPWMControl.prototype.getMotorSpeedPercentage = function () {
        return this.mMotorPercentage;
    };
    return MotorPWMControl;
})();
exports.MotorPWMControl = MotorPWMControl;
