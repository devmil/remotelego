var bleno = require('bleno');

export class LegoCarServiceAdvertiser {
    private mName: string;
    private mService;
    private mVisualizer: IBluetoothStateVisualizer;

    constructor(name: string, service, visualizer: IBluetoothStateVisualizer) {
        this.mName = name;
        this.mService = service;
        this.mVisualizer = visualizer;

        bleno.on('advertisingStart',
            (err) => {
                console.log('on -> advertisingStart: ' + (err ? 'error ' + err : 'success'));
                if (!err) {
                    console.log('advertising...');
                    //
                    // Once we are advertising, it's time to set up our services,
                    // along with our characteristics.
                    //
                    bleno.setServices([this.mService]);
                } else {
                    console.error(err);
                    this.start();
                }
            });
        bleno.on('advertisingStop', (err) => {
            console.log('on -> advertisingStop: ' + (err ? 'error ' + err : 'success'));
            
            this.start();
        });

        bleno.on('accept',
            (clientAddress) => {
                console.log("Accepted: " + clientAddress);
                this.mVisualizer.visualizeBluetoothClientConnection(true);
            });
        bleno.on('disconnect',
            (clientAddress) => {
                console.log("Disconnected: " + clientAddress);
                this.mVisualizer.visualizeBluetoothClientConnection(false);
            });
    }

    start() {
        this.advertiseServices();
    }
    
    stop() {
        bleno.stopAdvertising();
    }

    advertiseServices() {
        console.log("advertising services for " + this.mName);
        bleno.startAdvertising(this.mName, [this.mService.uuid]);
    }

}