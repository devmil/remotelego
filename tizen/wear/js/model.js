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
	this.lastUpdate = undefined;
	
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
	
	this.ensureConnectedPromise = function() {
		var instance = this;
		var result = new Promise(function(resolve, reject) {
			if(instance.isConnected) {
				resolve();
				return;
			}
			if(instance.isConnecting) {
				reject();
				return;
			}
			instance.device.connect(function onSuccess() {
				resolve();
			}, function onError() {
				reject();
			});
		});
		return result;
	};
	
	this.getService = function() {
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
	};
	
	this.setSpeed = function(speedPercent) {
		this.speed = speedPercent;
		this.transmitData();
	};
	
	this.setSteering = function(steeringPercent) {
		this.steering = steeringPercent;
		this.transmitData();
	};
	
	this.transmitSteeringPromise = function() {
		var instance = this;
		var steeringPromise = new Promise(
				function(resolve, reject) {
					if(instance.lastSentSteering === instance.steering) {
						resolve();
						return;
					}
					var sentSteering = instance.steering;
					var angle = (instance.steering * 90) / 100;

					var steeringData = new Array(1);
					steeringData[0] = Math.floor(angle);
					
					console.log("Sending steering (" + sentSteering + "%)");

					instance.getService().characteristics[1].writeValue(
							steeringData,
							function onSuccess() {
								console.log("Sending steering -- OK");
								instance.lastSentSteering = sentSteering;
								resolve();
							},
							function onError() {
								console.log("Sending steering -- FAIL");
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
					
					var sentSpeed = instance.speed;
					var speedData = new Array(1);
					speedData[0] = Math.floor(instance.speed);
					
					console.log("Sending speed (" + sentSpeed + "%)");
					instance.getService().characteristics[0].writeValue(
							speedData,
							function onSuccess() {
								console.log("Sending speed -- OK");
								instance.lastSentSpeed = sentSpeed;
								resolve();
							},
							function onError() {
								console.log("Sending speed -- FAIL");
								resolve();
							});
				});
		return speedPromise;
	};
	
	this.isDirty = function() {
		if(!this.lastUpdate) {
			return true;
		}
		return this.lastSentSpeed !== this.speed
			|| this.lastSentSteering !== this.steering;
	};
	
	this.transmitData = function() {
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
		var instance = this;
		this.ensureConnectedPromise()
		.then(this.transmitSteeringPromise())
		.then(this.transmitSpeedPromise())
		.then(function() {
			console.log("Transmitting finished");
			instance.lastUpdate = new Date();
		})
		.catch(function() {
		})
		.then(function() {
			instance.isTransmitting = false;
			if(instance.isDirty() && instance.isConnected) {
				window.setTimeout(function() {
					instance.transmitData();					
				}, 0);
			}			
		});
	};
}