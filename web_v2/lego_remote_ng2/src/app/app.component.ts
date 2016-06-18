import { Component } from '@angular/core';
import { MD_CARD_DIRECTIVES } from '@angular2-material/card';
import { MD_BUTTON_DIRECTIVES } from '@angular2-material/button';
import { MD_TOOLBAR_DIRECTIVES } from '@angular2-material/toolbar';
import { MD_SLIDE_TOGGLE_DIRECTIVES } from '@angular2-material/slide-toggle';
import { LegoCar } from './shared/LegoCar'
import { CarControlComponent } from "./control/control.component"
import { BluetoothDummy } from "./shared/BluetoothDummy"

@Component({
  moduleId: module.id,
  selector: 'app-root',
  templateUrl: 'app.component.html',
  styleUrls: ['app.component.css'],
  directives: [CarControlComponent, MD_TOOLBAR_DIRECTIVES, MD_BUTTON_DIRECTIVES, MD_SLIDE_TOGGLE_DIRECTIVES]
})
export class AppComponent {
  currentDevice: any = null;
  currentServer: any = null;
  currentModel: LegoCar = null;

  constructor() {
  }

  doConnect() {
    var serverTemp: any;
    var deviceTemp: any;

    new BluetoothDummy()
      //navigator.bluetooth

      .requestDevice({ filters: [{ services: ['40480f29-7bad-4ea5-8bf8-499405c9b324'] }] })
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

  onGattDisconnected() {
    this.deinit();
  }

  isConnected(){
    return this.currentDevice != null;
  }

  toggleConnectionStatus(){
    if(this.isConnected()){
      this.doDisconnect();
    }
    else{
      this.doConnect();
    }
  }

  doDisconnect() {
    if (this.currentDevice != null) {
      if (this.currentDevice.gatt.connected) {
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
    let carModel = new LegoCar(server, service, device.name);
    carModel.initData();
    this.currentModel = carModel;
  } 
}
