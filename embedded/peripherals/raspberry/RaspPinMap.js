var RaspPinMap = (function () {
    function RaspPinMap() {
    }
    RaspPinMap.getGpio = function (logicalPin) {
        var result = RaspPinMap.sPinGpioMapping[logicalPin];
        if (result == undefined) {
            throw "Unknown pin";
        }
        return result;
    };
    RaspPinMap.getPwm = function (logicalPin) {
        var result = RaspPinMap.sPwmPinMapping[logicalPin];
        if (result == undefined) {
            throw "Unknown pin";
        }
        return result;
    };
    RaspPinMap.sPinGpioMapping = {
        2: 3,
        3: 5,
        4: 7,
        14: 8,
        15: 10,
        17: 11,
        18: 12,
        27: 13,
        22: 15,
        23: 16,
        24: 18,
        10: 19,
        9: 21,
        25: 22,
        11: 23,
        8: 24,
        7: 26,
        5: 29,
        6: 31,
        12: 32,
        13: 33,
        19: 35,
        16: 36,
        26: 37,
        20: 38,
        21: 40
    };
    RaspPinMap.sPwmPinMapping = {
        12: 32,
        13: 33,
        18: 12,
        19: 35
    };
    return RaspPinMap;
})();
exports.RaspPinMap = RaspPinMap;