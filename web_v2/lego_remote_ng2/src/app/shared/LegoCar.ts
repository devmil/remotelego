import { Protocol, BlinkMode, FrontLightState, TrunkState } from './Protocol';

export class LegoCar {


    private m_server : any;
    private m_service : any;
    private m_name : string;

	private m_steering : number;
	private m_lastSentSteering : number;
	private m_speed : number;
	private m_lastSentSpeed : number;
	private m_trunkIsOpen : boolean;
	private m_lastSentTrunkIsOpen : boolean;
	private m_frontLightIsEnabled : boolean;
	private m_lastSentFrontLightIsEnabled : boolean;
    private m_blinkMode : BlinkMode =  BlinkMode.Off;
    private m_lastSentBlinkMode : BlinkMode = BlinkMode.Off;

    private m_isInitialized : boolean = false;
    private m_hasTrunkFeature : boolean = false;
    private m_hasFrontLightFeature : boolean = false;
    private m_hasBlinkFeature : boolean = false;
	
	private m_isTransmitting : boolean;

    constructor(server, service, name : string) {
        this.m_server = server;
        this.m_service = service;
        this.m_name = name;
    }

    public initData() {
        Promise.all( [
            this.m_service.getCharacteristic(Protocol.TRUNK_CHARACTERISTIC_UUID)
            .then((characteristic) => { this.m_hasTrunkFeature = true; })
            .catch((e) => { this.m_hasTrunkFeature = false; }),

            this.m_service.getCharacteristic(Protocol.FRONT_LIGHT_CHARACTERISTIC_UUID)
            .then((characteristic) => { this.m_hasFrontLightFeature = true; })
            .catch((e) => { this.m_hasFrontLightFeature = false; }),

            this.m_service.getCharacteristic(Protocol.BLINK_CHARACTERISTIC_UUID)
            .then((characteristic) => { this.m_hasBlinkFeature = true; })
            .catch((e) => { this.m_hasBlinkFeature = false; })
        ])
        .then((r) => { this.m_isInitialized = true; });
    }

    public get hasTrunkFeature() : boolean {
        return this.m_hasTrunkFeature;
    }

    public get hasFrontLightFeature() : boolean {
        return this.m_hasFrontLightFeature;
    }

    public get hasBlinkFeature() : boolean {
        return this.m_hasBlinkFeature;
    }

    public get steering() : number {
        return this.m_steering;
    }

    public set steering(steeringPercent : number) {
        this.m_steering = steeringPercent;
        this.transmitData();
    }

    public get speed() : number {
        return this.m_speed;
    }

    public set speed(speedPercent : number) {
        this.m_speed = speedPercent;
        this.transmitData();
    }

    public get trunkIsOpen() : boolean {
        return this.m_trunkIsOpen;
    }

    public set trunkIsOpen(isOpen : boolean) {
		this.m_trunkIsOpen = isOpen;
		this.transmitData();
    }

    public get frontLightIsEnabled() : boolean {
        return this.m_frontLightIsEnabled;
    }

    public set frontLightIsEnabled(isEnabled : boolean) {
		this.m_frontLightIsEnabled = isEnabled;
		this.transmitData();
    }

    public get blinkMode() : BlinkMode {
        return this.m_blinkMode;
    }

    public set blinkMode(blinkMode : BlinkMode) {
        this.m_blinkMode = blinkMode;
        this.transmitData();
    }

    public get blinkLeft() : boolean {
        return this.m_blinkMode == BlinkMode.Left || this.m_blinkMode == BlinkMode.Both;
    }

    public set blinkLeft(blinkLeft : boolean) {
        if(blinkLeft) {
            if(this.m_blinkMode == BlinkMode.Off) {
                this.blinkMode = BlinkMode.Left;
            } else if(this.m_blinkMode == BlinkMode.Right) {
                this.blinkMode = BlinkMode.Both;
            }
        } else {
            if(this.m_blinkMode == BlinkMode.Left) {
                this.blinkMode = BlinkMode.Off;
            } else if(this.m_blinkMode == BlinkMode.Both) {
                this.blinkMode = BlinkMode.Right;
            }
        }
    }

    public get blinkRight() : boolean {
        return this.m_blinkMode == BlinkMode.Right || this.m_blinkMode == BlinkMode.Both;
    }

    public set blinkRight(blinkRight : boolean) {
        if(blinkRight) {
            if(this.m_blinkMode == BlinkMode.Off) {
                this.blinkMode = BlinkMode.Right;
            } else if(this.m_blinkMode == BlinkMode.Left) {
                this.blinkMode = BlinkMode.Both;
            }
        } else {
            if(this.m_blinkMode == BlinkMode.Right) {
                this.blinkMode = BlinkMode.Off;
            } else if(this.m_blinkMode == BlinkMode.Both) {
                this.blinkMode = BlinkMode.Left;
            }
        }
    }

    private updateIfDirty() {
		if(this.isDirty() && this.m_server.connected) {
			window.setTimeout(() => { this.transmitData(); }, 0);
		}
	};

    private transmitDataPromise(characteristicUUID : string, checkDirtyAction, getBytesAction, successAction) {
        return this.m_service.getCharacteristic(characteristicUUID)
        .then((characteristic) => {
            if(checkDirtyAction()) {
                var data: Uint8Array = getBytesAction();
                return characteristic.writeValue(data);
            }
            return new Promise((resolve, reject) => { reject(); });
        })
        .then(() => {
            successAction();
        })
        .catch(() => {});
    }

    private isDirty() : boolean {
        return this.m_lastSentSpeed !== this.speed
			|| this.m_lastSentSteering !== this.steering
			|| this.m_lastSentTrunkIsOpen !== this.trunkIsOpen
			|| this.m_lastSentFrontLightIsEnabled !== this.frontLightIsEnabled
            || this.m_lastSentBlinkMode != this.blinkMode;
    }

    private transmitData() {
        console.log("Got transmit request");
		if(this.m_isTransmitting) {
			console.log("Is currently transmitting => rejecting request");
			return;
		}
		if(!this.isDirty()) {
			console.log("Nothing dirty, don't transmit");
			return;
		}
		this.m_isTransmitting = true;

        var l_steering = this.steering;
        var l_speed = this.speed;
        var l_trunkIsOpen = this.trunkIsOpen;
        var l_frontLightIsEnabled = this.frontLightIsEnabled
        var l_blinkMode = this.blinkMode;

        Promise.all([
            this.transmitDataPromise(
                Protocol.STEER_CHARACTERISTIC_UUID,
                () => { return l_steering != this.m_lastSentSteering; },
                () => {  
					var angle = (l_steering * 90) / 100;

					var steeringData = new Uint8Array(1);
					steeringData[0] = Math.floor(angle);
					
					console.log("Sending steering (" + l_steering + "%)");

                    return steeringData;
                },
                () => {
                    this.m_lastSentSteering = l_steering;
                    console.log("Sent steering (" + l_steering + "%)");
                }),

            this.transmitDataPromise(
                Protocol.SPEED_CHARACTERISTIC_UUID,
                () => { return l_speed != this.m_lastSentSpeed; },
                () => {  
					var speedData = new Uint8Array(1);
					speedData[0] = Math.floor(l_speed);
					
					console.log("Sending speed (" + l_speed + "%)");

                    return speedData;
                },
                () => {
                    this.m_lastSentSpeed = l_speed;
                    console.log("Sent speed (" + l_speed + "%)");
                }),
                
            this.transmitDataPromise(
                Protocol.TRUNK_CHARACTERISTIC_UUID,
                () => { return this.hasTrunkFeature && l_trunkIsOpen != this.m_lastSentTrunkIsOpen; },
                () => {  
					var trunkData = new Uint8Array(1);
					trunkData[0] = l_trunkIsOpen ? TrunkState.Open : TrunkState.Closed;
					
					console.log("Sending trunk state (" + l_trunkIsOpen + ")");

                    return trunkData;
                },
                () => {
                    this.m_lastSentTrunkIsOpen = l_trunkIsOpen;
					console.log("Sent trunk state (" + l_trunkIsOpen + ")");
                }),

            this.transmitDataPromise(
                Protocol.FRONT_LIGHT_CHARACTERISTIC_UUID,
                () => { return this.hasFrontLightFeature && l_frontLightIsEnabled != this.m_lastSentFrontLightIsEnabled; },
                () => {  
					var frontLightData = new Uint8Array(1);
					frontLightData[0] = l_frontLightIsEnabled ? FrontLightState.Active : FrontLightState.Hidden;
					
					console.log("Sending front light state (" + l_frontLightIsEnabled + ")");
                    return frontLightData;
                },
                () => {
                    this.m_lastSentFrontLightIsEnabled = l_frontLightIsEnabled;
					console.log("Sent front light state (" + l_frontLightIsEnabled + ")");
                }),

            this.transmitDataPromise(
                Protocol.BLINK_CHARACTERISTIC_UUID,
                () => { return this.hasBlinkFeature && l_blinkMode != this.m_lastSentBlinkMode; },
                () => {  
					var blinkModeData = new Uint8Array(1);
					blinkModeData[0] = l_blinkMode;
					
					console.log("Sending blink mode (" + l_blinkMode + ")");
                    return blinkModeData;
                },
                () => {
                    this.m_lastSentBlinkMode = l_blinkMode;
					console.log("Sent blink mode (" + l_blinkMode + ")");
                }),
        ])
        .catch(() => {
        })
        .then(() => {
            this.m_isTransmitting = false;
            this.updateIfDirty();
        });
    }
}