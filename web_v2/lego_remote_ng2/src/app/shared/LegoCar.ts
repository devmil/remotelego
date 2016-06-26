export class LegoCar {

    static SPEED_CHARACTERISTIC_UUID : string = "8d8ba32b-96be-4590-910b-c756c5222c9f";
	static STEER_CHARACTERISTIC_UUID : string = "7baf8dca-2bfc-47fb-af29-042fccc180eb";
    static TRUNK_CHARACTERISTIC_UUID : string = "e0af3340-022e-47e1-a263-d68887dc41d4";
    static FRONT_LIGHT_CHARACTERISTIC_UUID : string = "fa10e4de-259e-4d23-9f59-45a9c66802ca";
    static BLINK_CHARACTERISTIC_UUID : string = "aad03b81-f2ea-47db-ae1e-7c2f9e86e93e";

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
            this.m_service.getCharacteristic(LegoCar.TRUNK_CHARACTERISTIC_UUID)
            .then((characteristic) => { this.m_hasTrunkFeature = true; })
            .catch((e) => { this.m_hasTrunkFeature = false; }),

            this.m_service.getCharacteristic(LegoCar.FRONT_LIGHT_CHARACTERISTIC_UUID)
            .then((characteristic) => { this.m_hasFrontLightFeature = true; })
            .catch((e) => { this.m_hasFrontLightFeature = false; }),

            this.m_service.getCharacteristic(LegoCar.BLINK_CHARACTERISTIC_UUID)
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
			|| this.m_lastSentFrontLightIsEnabled !== this.frontLightIsEnabled;
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

        Promise.all([
            this.transmitDataPromise(
                LegoCar.STEER_CHARACTERISTIC_UUID,
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
                LegoCar.SPEED_CHARACTERISTIC_UUID,
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
                LegoCar.TRUNK_CHARACTERISTIC_UUID,
                () => { return this.hasTrunkFeature && l_trunkIsOpen != this.m_lastSentTrunkIsOpen; },
                () => {  
					var trunkData = new Uint8Array(1);
					trunkData[0] = l_trunkIsOpen ? 2 : 1;
					
					console.log("Sending trunk state (" + l_trunkIsOpen + ")");

                    return trunkData;
                },
                () => {
                    this.m_lastSentTrunkIsOpen = l_trunkIsOpen;
					console.log("Sent trunk state (" + l_trunkIsOpen + ")");
                }),

            this.transmitDataPromise(
                LegoCar.FRONT_LIGHT_CHARACTERISTIC_UUID,
                () => { return this.hasFrontLightFeature && l_frontLightIsEnabled != this.m_lastSentFrontLightIsEnabled; },
                () => {  
					var frontLightData = new Uint8Array(1);
					frontLightData[0] = l_frontLightIsEnabled ? 2 : 1;
					
					console.log("Sending front light state (" + l_frontLightIsEnabled + ")");
                    return frontLightData;
                },
                () => {
                    this.m_lastSentFrontLightIsEnabled = l_frontLightIsEnabled;
					console.log("Sent front light state (" + l_frontLightIsEnabled + ")");
                }),

                //TODO: blink
        ])
        .catch(() => {
        })
        .then(() => {
            this.m_isTransmitting = false;
            this.updateIfDirty();
        });
    }
}