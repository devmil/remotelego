import { Component } from '@angular/core';
import { LegoCar } from './shared/LegoCar'
import { CarControlComponent } from "./control.component"
import { BluetoothDummy } from "./shared/BluetoothDummy"

@Component({
  selector: 'lego-control',
  template: `
    <h1>LegoControl app</h1>
    <div *ngIf="!currentModel" align="center">
        <div>Click connect to connect to a vehicle</div>
        <div><button (click)="doConnect()">Connect</button></div>
    </div>
    <div *ngIf="currentModel">
        <div><button (click)="doDisconnect()">Disconnect</button></div>
        <car-control [model]="currentModel"></car-control>
    </div>
    `,
    directives: [CarControlComponent]
})
export class AppComponent 
{ 
    currentDevice : any = null;
    currentServer : any = null;
    currentModel : LegoCar = null;

    constructor() {
    }

    doConnect() {
        var serverTemp : any;
        var deviceTemp : any;

        new BluetoothDummy()
        //navigator.bluetooth
        
        .requestDevice({ filters: [{ services: [ '40480f29-7bad-4ea5-8bf8-499405c9b324' ] }] })
        .then(device => {
            deviceTemp = device;
            device.addEventListener('gattserverdisconnected', this.onGattDisconnected);
            return device.gatt.connect()
        })
        .then(server => { 
            serverTemp = server;
            return server.getPrimaryService('40480f29-7bad-4ea5-8bf8-499405c9b324');
        })
        .then(service => {
            this.init(service, deviceTemp, serverTemp);
        })
        .catch(error => { 
            console.log(error);
            this.doDisconnect(); 
        })
    }

    onGattDisconnected () {
        this.deinit();
    }

    doDisconnect() {
        if(this.currentDevice != null) {
            if(this.currentDevice.gatt.connected) {
                this.currentDevice.gatt.disconnect();
            }
        }
        this.deinit();
    }

    deinit() {
        this.currentDevice = null;
        this.currentServer = null;
        this.currentModel = null;
    }

    init(service, device, server) {
        this.currentDevice = device;
        this.currentServer = server;
        this.currentModel = new LegoCar(server, service, device.name);
    }
}