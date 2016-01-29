var EdisonPinMap = (function () {
    function EdisonPinMap() {
    }
    EdisonPinMap.getGpio = function (linuxPin) {
        var result = EdisonPinMap.sPinGpioMapping[linuxPin];
        if (result == undefined) {
            throw "Unknown pin";
        }
        return result;
    };
    EdisonPinMap.getPwm = function (linuxPin) {
        var result = EdisonPinMap.sPwmPinMapping[linuxPin];
        if (result == undefined) {
            throw "Unknown pin";
        }
        return result;
    };
    //mapping taken from https://learn.sparkfun.com/tutorials/installing-libmraa-on-ubilinux-for-edison
    EdisonPinMap.sPinGpioMapping = {
        12: 20,
        13: 14,
        14: 36,
        15: 48,
        19: 19,
        20: 7,
        27: 6,
        28: 8,
        40: 37,
        41: 51,
        42: 50,
        43: 38,
        44: 31,
        45: 45,
        46: 32,
        47: 46,
        48: 33,
        49: 47,
        77: 39,
        78: 52,
        79: 53,
        80: 54,
        81: 55,
        82: 40,
        83: 41,
        84: 49,
        109: 10,
        110: 23,
        111: 9,
        114: 24,
        115: 11,
        128: 13,
        129: 25,
        130: 26,
        131: 35,
        134: 44,
        135: 4,
        165: 15,
        182: 0,
        183: 21
    };
    EdisonPinMap.sPwmPinMapping = {
        12: 20,
        13: 14,
        182: 0,
        183: 21
    };
    return EdisonPinMap;
})();
exports.EdisonPinMap = EdisonPinMap;
