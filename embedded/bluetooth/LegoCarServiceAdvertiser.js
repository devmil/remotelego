var bleno = require('bleno');
var LegoCarServiceAdvertiser = (function () {
    function LegoCarServiceAdvertiser(name, service, visualizer) {
        var _this = this;
        this.mName = name;
        this.mService = service;
        this.mVisualizer = visualizer;
        bleno.on('advertisingStart', function (err) {
            console.log('on -> advertisingStart: ' + (err ? 'error ' + err : 'success'));
            if (!err) {
                console.log('advertising...');
                //
                // Once we are advertising, it's time to set up our services,
                // along with our characteristics.
                //
                bleno.setServices([_this.mService]);
            }
            else {
                console.error(err);
                _this.start();
            }
        });
        bleno.on('advertisingStop', function (err) {
            console.log('on -> advertisingStop: ' + (err ? 'error ' + err : 'success'));
            _this.start();
        });
        bleno.on('accept', function (clientAddress) {
            console.log("Accepted: " + clientAddress);
            _this.mVisualizer.visualizeBluetoothClientConnection(true);
        });
        bleno.on('disconnect', function (clientAddress) {
            console.log("Disconnected: " + clientAddress);
            _this.mVisualizer.visualizeBluetoothClientConnection(false);
        });
    }
    LegoCarServiceAdvertiser.prototype.start = function () {
        this.advertiseServices();
    };
    LegoCarServiceAdvertiser.prototype.stop = function () {
        bleno.stopAdvertising();
    };
    LegoCarServiceAdvertiser.prototype.advertiseServices = function () {
        console.log("advertising services for " + this.mName);
        bleno.startAdvertising(this.mName, [this.mService.uuid]);
    };
    return LegoCarServiceAdvertiser;
})();
exports.LegoCarServiceAdvertiser = LegoCarServiceAdvertiser;
