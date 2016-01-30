var bleno = require('bleno');
var eddystoneBeacon = require('eddystone-beacon');
function advertiseEddyStone() {
    console.log("advertising eddystone");
    var eddyStoneOptions = {
        txPowerLevel: -21,
        tlmCount: 2,
        tlmPeriod: 100 // every 100 advertisements => every 10 seconds (doesn't get used here)
    };
    var url = 'https://goo.gl/2uYtBd'; //TODO: echange with the lego remote app URL
    eddystoneBeacon.advertiseUrl(url, eddyStoneOptions);
}
function potentiallyDelayedStart() {
    if (bleno.state == 'poweredOn') {
        advertiseEddyStone();
    }
    else {
        setTimeout(function () {
            potentiallyDelayedStart();
        }, 2000);
    }
}
bleno.on('stateChange', function (state) {
    console.log('bleno state: ' + state);
    if ('poweredOn' == state) {
        potentiallyDelayedStart();
    }
});
function checkIfDeviceIsUpAndStart() {
    var isDeviceUp = bleno._bindings._hci._socket.isDevUp();
    if (!isDeviceUp) {
        console.log('Device isn\'t up. Retrying later');
        setTimeout(function () {
            process.exit(0);
        }, 2000);
    }
    else {
        console.log('Device is up. Starting...');
        bleno._bindings.init();
        bleno._bindings._hci.emit('stateChange', 'poweredOn');
    }
}
checkIfDeviceIsUpAndStart();
