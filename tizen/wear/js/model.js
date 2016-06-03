function CarModel (device) {
	this.SPEED_CHARACTERISTIC_UUID = "8d8ba32b-96be-4590-910b-c756c5222c9f";
	this.STEER_CHARACTERISTIC_UUID = "7baf8dca-2bfc-47fb-af29-042fccc180eb";
	this.device = device;
	this.service = undefined;
	
	this.isConnecting = false;
	this.isConnected = false;
	
	this.steering = undefined;
	this.lastSentSteering = undefined;
	this.speed = undefined;
	this.lastSentSpeed = undefined;
	
	this.isTransmitting = false;
	
	this.connect = function() {
		if(this.isConnecting || this.isConnected) {
			return;
		}
		var instance = this;
		this.device.connect(
			function onConnectionSuccess() {
				instance.isConnected = true;
				instance.isConnecting = false;
			},
			function onConnectionFailed() {
				instance.isConnected = false;
				instance.isConnecting = false;
			});
	};
	
	this.ensureConnected = function() {
		if(this.isConnected || this.isConnecting) {
			return;
		}
		this.connect();
	};
	
	this.getService = function() {
		this.ensureConnected();
		if(!this.service && device) {
			this.service = device.getService("40480f29-7bad-4ea5-8bf8-499405c9b324");
		}
		return this.service;
	};
	
	this.disconnect = function() {
		this.device.disconnect();
		this.service = undefined;
		this.isConnected = false;
		this.isConnecting = false;
		if(this.sendTimeout) {
			window.clearTimeout(this.sendTimeout);
		}
	};
	
	this.setSpeed = function(speedPercent) {
		if(this.lastSentSpeed === speedPercent) {
			return;
		}
		this.speed = speedPercent;
		this.transmitData();
	};
	
	this.setSteering = function(steeringPercent) {
		if(this.lastSentSteering === steeringPercent) {
			return;
		}
		this.steering = steeringPercent;
		this.transmitData();
	};
	
	this.ensureConnectionPromise = function() {
		var instance = this;
		var result = new Promise(
				function(resolve, reject) {
					instance.ensureConnected();
					if(!instance.isConnected) {
						reject();
					} else {
						resolve();
					}
				});
		return result;
	};
	
	this.transmitSteeringPromise = function() {
		var instance = this;
		var steeringPromise = new Promise(
				function(resolve, reject) {
					if(instance.lastSentSteering === instance.steering) {
						resolve();
						return;
					}
					var angle = (instance.steering * 90) / 100;

					var steeringData = new Array(1);
					steeringData[0] = Math.floor(angle);

					instance.getService().characteristics[1].writeValue(
							steeringData,
							function onSuccess() {
								instance.lastSentSteering = instance.steering;
								resolve();
							},
							function onError() {
								resolve();
							});
				});
		return steeringPromise;
	};
	
	this.transmitSpeedPromise = function() {
		var instance = this;
		var speedPromise = new Promise(
				function(resolve, reject) {
					if(instance.lastSentSpeed === instance.speed) {
						resolve();
						return;
					}
					
					var speedData = new Array(1);
					speedData[0] = Math.floor(instance.speed);
					
					instance.getService().characteristics[0].writeValue(
							speedData,
							function onSuccess() {
								instance.lastSentSpeed = instance.speed;
								resolve();
							},
							function onError() {
								resolve();
							});
				});
		return speedPromise;
	};
	
	this.isDirty = function() {
		return this.lastSentSpeed !== this.speed
			|| this.lastSentSteering !== this.steering;
	};
	
	this.transmitData = function() {
		if(this.isTransmitting) {
			return;
		}
		if(!this.isDirty()) {
			return;
		}
		var instance = this;
		this.ensureConnectionPromise()
		.then(this.transmitSteeringPromise())
		.then(this.transmitSpeedPromise())
		.then(function() {
			instance.isTransmitting = false;
			if(instance.isDirty()) {
				window.setTimeout(function() {
					instance.transmitData();					
				}, 100);
			}
		});
	};
}