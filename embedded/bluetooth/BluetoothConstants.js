var BluetoothConstants = (function () {
    function BluetoothConstants() {
    }
    Object.defineProperty(BluetoothConstants, "SERVICE_LEGOCAR_GUID", {
        get: function () {
            return BluetoothConstants.sServiceLegoCarGuid;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BluetoothConstants, "CHARACTERISTIC_SPEED_GUID", {
        get: function () {
            return BluetoothConstants.sCharacteristicSpeedGuid;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BluetoothConstants, "CHARACTERISTIC_STEER_GUID", {
        get: function () {
            return BluetoothConstants.sCharacteristicSteerGuid;
        },
        enumerable: true,
        configurable: true
    });
    BluetoothConstants.sServiceLegoCarGuid = '40480f29-7bad-4ea5-8bf8-499405c9b324';
    BluetoothConstants.sCharacteristicSpeedGuid = '8d8ba32b-96be-4590-910b-c756c5222c9f';
    BluetoothConstants.sCharacteristicSteerGuid = '7baf8dca-2bfc-47fb-af29-042fccc180eb';
    return BluetoothConstants;
})();
exports.BluetoothConstants = BluetoothConstants;
