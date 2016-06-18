export class LegoCar {

    static SPEED_CHARACTERISTIC_UUID : string = "8d8ba32b-96be-4590-910b-c756c5222c9f";
	static STEER_CHARACTERISTIC_UUID : string = "7baf8dca-2bfc-47fb-af29-042fccc180eb";
    static TRUNK_CHARACTERISTIC_UUID : string = "e0af3340-022e-47e1-a263-d68887dc41d4";
    static FRONT_LIGHT_CHARACTERISTIC_UUID : string = "fa10e4de-259e-4d23-9f59-45a9c66802ca";
    static BLINK_CHARACTERISTIC_UUID : string = "aad03b81-f2ea-47db-ae1e-7c2f9e86e93e";

    m_server : any;
    m_service : any;
    m_name : string;

	steering : number;
	lastSentSteering : number;
	speed : number;
	lastSentSpeed : number;
	trunkIsOpen : boolean;
	lastSentTrunkIsOpen : boolean;
	frontLightIsEnabled : boolean;
	lastSentFrontLightIsEnabled : boolean;

    isInitialized : boolean = false;
    hasTrunkFeature : boolean = false;
    hasFrontLightFeature : boolean = false;
    hasBlinkFeature : boolean = false;
	
	isTransmitting : boolean;

    constructor(server, service, name : string) {
        this.m_server = server;
        this.m_service = service;
        this.m_name = name;
    }

    initData() {
        Promise.all( [
            this.m_service.getCharacteristic(LegoCar.TRUNK_CHARACTERISTIC_UUID)
            .then((characteristic) => { this.hasTrunkFeature = true; })
            .catch((e) => { this.hasTrunkFeature = false; }),

            this.m_service.getCharacteristic(LegoCar.FRONT_LIGHT_CHARACTERISTIC_UUID)
            .then((characteristic) => { this.hasFrontLightFeature = true; })
            .catch((e) => { this.hasFrontLightFeature = false; }),

            this.m_service.getCharacteristic(LegoCar.BLINK_CHARACTERISTIC_UUID)
            .then((characteristic) => { this.hasBlinkFeature = true; })
            .catch((e) => { this.hasBlinkFeature = false; })
        ])
        .then((r) => { this.isInitialized = true; });
    }

    setSteering(steeringPercent: number) {
        this.steering = steeringPercent;
        this.transmitData();
    }

    setSpeed(speedPercent : number) {
        this.speed = speedPercent;
        this.transmitData();
    }

    setTrunkOpen(isOpen : boolean) {
		this.trunkIsOpen = isOpen;
		this.transmitData();
	};
	
	setFrontLightIsEnabled(isEnabled : boolean) {
		this.frontLightIsEnabled = isEnabled;
		this.transmitData();
	};

    updateIfDirty() {
		if(this.isDirty() && this.m_server.connected) {
			window.setTimeout(() => { this.transmitData(); }, 0);
		}
	};

    transmitDataPromise(characteristicUUID : string, checkDirtyAction, getBytesAction, successAction) {
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

    isDirty() : boolean {
        return this.lastSentSpeed !== this.speed
			|| this.lastSentSteering !== this.steering
			|| this.lastSentTrunkIsOpen !== this.trunkIsOpen
			|| this.lastSentFrontLightIsEnabled !== this.frontLightIsEnabled;
    }

    transmitData() {
        console.log("Got transmit request");
		if(this.isTransmitting) {
			console.log("Is currently transmitting => rejecting request");
			return;
		}
		if(!this.isDirty()) {
			console.log("Nothing dirty, don't transmit");
			return;
		}
		this.isTransmitting = true;

        var steering = this.steering;
        var speed = this.speed;
        var trunkIsOpen = this.trunkIsOpen;
        var frontLightIsEnabled = this.frontLightIsEnabled

        Promise.all([
            this.transmitDataPromise(
                LegoCar.STEER_CHARACTERISTIC_UUID,
                () => { return steering != this.lastSentSteering; },
                () => {  
					var angle = (steering * 90) / 100;

					var steeringData = new Uint8Array(1);
					steeringData[0] = Math.floor(angle);
					
					console.log("Sending steering (" + steering + "%)");

                    return steeringData;
                },
                () => {
                    this.lastSentSteering = steering;
                    console.log("Sent steering (" + steering + "%)");
                }),

            this.transmitDataPromise(
                LegoCar.SPEED_CHARACTERISTIC_UUID,
                () => { return speed != this.lastSentSpeed; },
                () => {  
					var speedData = new Uint8Array(1);
					speedData[0] = Math.floor(speed);
					
					console.log("Sending speed (" + speed + "%)");

                    return speedData;
                },
                () => {
                    this.lastSentSpeed = speed;
                    console.log("Sent speed (" + speed + "%)");
                }),
                
            this.transmitDataPromise(
                LegoCar.TRUNK_CHARACTERISTIC_UUID,
                () => { return this.hasTrunkFeature && trunkIsOpen != this.lastSentTrunkIsOpen; },
                () => {  
					var trunkData = new Uint8Array(1);
					trunkData[0] = trunkIsOpen ? 1 : 0;
					
					console.log("Sending trunk state (" + trunkIsOpen + ")");

                    return trunkData;
                },
                () => {
                    this.lastSentTrunkIsOpen = trunkIsOpen;
					console.log("Sent trunk state (" + trunkIsOpen + ")");
                }),

            this.transmitDataPromise(
                LegoCar.FRONT_LIGHT_CHARACTERISTIC_UUID,
                () => { return this.hasFrontLightFeature && frontLightIsEnabled != this.lastSentFrontLightIsEnabled; },
                () => {  
					var frontLightData = new Uint8Array(1);
					frontLightData[0] = frontLightIsEnabled ? 1 : 0;
					
					console.log("Sending front light state (" + frontLightIsEnabled + ")");
                    return frontLightData;
                },
                () => {
                    this.lastSentFrontLightIsEnabled = frontLightIsEnabled;
					console.log("Sent front light state (" + frontLightIsEnabled + ")");
                }),

                //TODO: blink
        ])
        .catch(() => {
        })
        .then(() => {
            this.isTransmitting = false;
            this.updateIfDirty();
        });
    }
}