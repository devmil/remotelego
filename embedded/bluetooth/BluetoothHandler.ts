var bleno = require('bleno');
var LegoCarService = require('./LegoCarService');
import adv = require('./LegoCarServiceAdvertiser');

export class BluetoothHandler {
    private mCarModel: ICarModel;
    private mLegoCarService;
    private mAdvertiser : adv.LegoCarServiceAdvertiser;
    
    constructor(carModel: ICarModel, visualizer: IBluetoothStateVisualizer) {
        this.mCarModel = carModel;
        this.mLegoCarService = new LegoCarService(carModel);
        this.mAdvertiser = new adv.LegoCarServiceAdvertiser("Lego Truck", this.mLegoCarService, visualizer);
        
        bleno.on('stateChange',
            (state) =>
            {
                console.log('bleno state: ' + state);
                if ('poweredOn' == state) {
                    this.mAdvertiser.start();
                } else {
                    this.mAdvertiser.stop();
                }
            });
    } 
}