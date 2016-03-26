var ServoPWMControl = (function () {
    function ServoPWMControl(peripheralAccess, pin, reverseServo) {
        this.mPeripheralAccess = peripheralAccess;
        this.mPin = peripheralAccess.getPwm(pin);
        this.mReverse = reverseServo;
        this.prepare();
        this.setAngle(0);
    }
    ServoPWMControl.prototype.prepare = function () {
        //configure PWM
        this.mPin.open(ServoPWMControl.sRangeMSec * 1000);
    };
    ServoPWMControl.prototype.setAngle = function (angle) {
        //check range -90 <= angle >= 90
        if (angle < -90) {
            angle = -90;
        }
        else if (angle > 90) {
            angle = 90;
        }
        this.mAngle = angle;
        if (this.mReverse) {
            angle *= -1;
        }
        var ratio = ((angle + 90) / 180);
        var activeMSec = ((ServoPWMControl.sPulseMaxMSec - ServoPWMControl.sPulseMinMSec) * ratio) + ServoPWMControl.sPulseMinMSec;
        var pwmPercentage = activeMSec / ServoPWMControl.sRangeMSec;
        this.mPin.setPercentage(pwmPercentage);
        console.log('Angle = ' + this.mAngle + ', Ratio = ' + ratio + ', Active ms = ' + activeMSec);
    };
    ServoPWMControl.prototype.getAngle = function () {
        return this.mAngle;
    };
    ServoPWMControl.sRangeMSec = 20;
    ServoPWMControl.sPulseMinMSec = 0.5;
    ServoPWMControl.sPulseMaxMSec = 2.45;
    return ServoPWMControl;
})();
exports.ServoPWMControl = ServoPWMControl;
