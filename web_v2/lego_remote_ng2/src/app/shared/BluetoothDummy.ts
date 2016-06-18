export class BluetoothDummyCharacteristic {

    uuid : string;

    constructor(uuid : string) {
        this.uuid = uuid;
    }

    writeValue(data : Uint8Array) {
        return new Promise((resolve, reject) => {
            console.log("Writing " + data + " to characteristic [" + this.uuid +  "]");
            resolve();
        });
    }
}

export class BluetoothDummyGattService {
    getCharacteristic(uuid : string) {
        return new Promise<BluetoothDummyCharacteristic>((resolve, reject) => { resolve(new BluetoothDummyCharacteristic(uuid)) });
    }
}

export class BluetoothDummyGattServer {
    connected : boolean = false;
    
    connect() {
        return new Promise<BluetoothDummyGattServer>((resolve, reject) => { this.connected = true; resolve(this) });
    }

    disconnect() {
        this.connected = false;
    }

    getPrimaryService(uuid : string) {
        return new Promise<BluetoothDummyGattService>((resolve, reject) => { resolve(new BluetoothDummyGattService()) });
    }
}

export class BluetoothDummyDevice {
    gatt : BluetoothDummyGattServer = new BluetoothDummyGattServer();

    addEventListener(event : string, listener : any) {
        console.log("Added listener to event: [" + event + "]");
    }
}

export class BluetoothDummyRequestFilter {
    services : string[];
}

export class BluetoothDummyRequestParams {
    filters : BluetoothDummyRequestFilter[];
}

export class BluetoothDummy {
    requestDevice(params : BluetoothDummyRequestParams) {
        console.log("Request device with " + params + " called");
        return new Promise<BluetoothDummyDevice>((resolve, reject) => { resolve(new BluetoothDummyDevice()); })
    }
}