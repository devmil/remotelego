var bleno = require('bleno');
var LegoCarService = require('./LegoCarService');
var adv = require('./LegoCarServiceAdvertiser');
var BluetoothHandler = (function () {
    function BluetoothHandler(carModel, visualizer) {
        var _this = this;
        this.mCarModel = carModel;
        this.mLegoCarService = new LegoCarService(carModel);
        this.mAdvertiser = new adv.LegoCarServiceAdvertiser("Lego Truck", this.mLegoCarService, visualizer);
        bleno.on('stateChange', function (state) {
            console.log('bleno state: ' + state);
            if ('poweredOn' == state) {
                _this.mAdvertiser.start();
            }
            else {
                _this.mAdvertiser.stop();
            }
        });
    }
    return BluetoothHandler;
})();
exports.BluetoothHandler = BluetoothHandler;
