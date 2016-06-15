function CarModel (device) {
	this.SPEED_CHARACTERISTIC_UUID = "8d8ba32b-96be-4590-910b-c756c5222c9f";
	this.STEER_CHARACTERISTIC_UUID = "7baf8dca-2bfc-47fb-af29-042fccc180eb";
	this.device = device;
	this.service = undefined;
	
	var listenerInstance = this;
	
	//this leads to a no more connectable device
	/*this.device.addConnectStateChangeListener(function Listener() {
		this.onconnected = function(device) {
			listenerInstance.isConnected = true;
			listenerInstance.isConnecting = false;
		};
		this.ondisconnected = function(device) {
			listenerInstance.isConnected = false;
			listenerInstance.isConnecting = false;			
		};
	});*/
	
	this.isConnecting = false;
	this.isConnected = false;
	
	this.steering = undefined;
	this.lastSentSteering = undefined;
	this.speed = undefined;
	this.lastSentSpeed = undefined;
	this.trunkIsOpen = undefined;
	this.lastSentTrunkIsOpen = undefined;
	this.frontLightIsEnabled = undefined;
	this.lastSentFrontLightIsEnabled = undefined;
	
	this.lastUpdate = undefined;
	
	this.isTransmitting = false;
	
	this.connect = function() {
		if(this.isConnected || this.isConnecting) {
			return;
		}
		var instance = this;
		instance.isConnecting = true;
		this.device.connect(
			function onSuccess() {
				instance.isConnected = true;
				instance.isConnecting = false;
				instance.updateIfDirty();
			}, 
			function onError() {
				instance.isConnected = false;
				instance.isConnecting = false;
			}
		);
	};
	
	this.ensureConnected = function() {
		if(this.isConnected || this.isConnecting) {
			return;
		}
		this.connect();
	};
	
	this.getService = function() {
		if(!this.service && device) {
			this.service = device.getService("40480f29-7bad-4ea5-8bf8-499405c9b324");
		}
		return this.service;
	};
	
	this.disconnect = function() {
		if(this.device) {
			this.device.disconnect();
		}
		this.service = undefined;
		this.isConnected = false;
		this.isConnecting = false;
		this.lastSentFrontLightIsEnabled = undefined;
		this.lastSentSpeed = undefined;
		this.lastSentSteering = undefined;
		this.lastSentTrunkIsOpen = undefined;
	};
	
	this.setSpeed = function(speedPercent) {
		this.speed = speedPercent;
		this.transmitData();
	};
	
	this.setSteering = function(steeringPercent) {
		this.steering = steeringPercent;
		this.transmitData();
	};
	
	this.setTrunkOpen = function(isOpen) {
		this.trunkIsOpen = isOpen;
		this.transmitData();
	};
	
	this.setFrontLightIsEnabled = function(isEnabled) {
		this.frontLightIsEnabled = isEnabled;
		this.transmitData();
	};
	
	this.hasTrunkFunction = function() {
		if(this.service) {
			//TODO: whenever the API exposes the UUID of the characteristics use the UUID instead
			//		or make a new characteristic (on index 1) that describes the feature set
			return (this.service.characteristics.length > 2);
		}
		return false;
	};
	
	this.hasFrontLightFunction = function() {
		if(this.service) {
			//TODO: whenever the API exposes the UUID of the characteristics use the UUID instead
			//		or make a new characteristic (on index 1) that describes the feature set
			return (this.service.characteristics.length > 3);
		}
		return false;
	};
	
	this.updateIfDirty = function() {
		var instance = this;
		if(instance.isDirty() && instance.isConnected) {
			window.setTimeout(function() {
				instance.transmitData();					
			}, 0);
		}
	};
	
	this.transmitSteeringPromise = function() {
		var instance = this;
		var steeringPromise = new Promise(
				function(resolve, reject) {
					if(instance.lastSentSteering === instance.steering) {
						return resolve();
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
						return resolve();
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
	
	this.transmitTrunkStatePromise = function() {
		var instance = this;
		var trunkStatePromise = new Promise(
				function(resolve, reject) {
					if(!instance.hasTrunkFunction()) {
						return resolve();
					}

					if(instance.lastSentTrunkIsOpen === instance.trunkIsOpen) {
						return resolve();
					}
					var sentTrunkIsOpen = instance.trunkIsOpen;
					var trunkData = new Array(1);
					trunkData[0] = sentTrunkIsOpen ? 1 : 0;
					
					console.log("Sending trunk state (" + sentTrunkIsOpen + ")");
					if(instance.getService().characteristics.length > 2) {
						instance.getService().characteristics[2].writeValue(
								trunkData,
								function onSuccess() {
									console.log("Sending trunk state -- OK");
									instance.lastSentTrunkIsOpen = sentTrunkIsOpen;
									resolve();
								},
								function onError() {
									console.log("Sending trunk state -- FAIL");
									resolve();
								});
					} else {
						console.log("Not enough characteristics to send trunk state");
						resolve();
					}
				});
		return trunkStatePromise;
	};
	
	this.transmitFrontLightStatePromise = function() {
		var instance = this;
		var frontLightStatePromise = new Promise(
				function(resolve, reject) {
					if(!instance.hasFrontLightFunction()) {
						return resolve();
					}
					if(instance.lastSentFrontLightIsEnabled === instance.frontLightIsEnabled) {
						return resolve();
					}
					var sentFrontLightIsEnabled = instance.frontLightIsEnabled;
					var frontLightData = new Array(1);
					frontLightData[0] = sentFrontLightIsEnabled ? 1 : 0;
					
					console.log("Sending front light state (" + sentFrontLightIsEnabled + ")");
					if(instance.getService().characteristics.length > 3) {
						instance.getService().characteristics[3].writeValue(
								frontLightData,
								function onSuccess() {
									console.log("Sending front light state -- OK");
									instance.lastSentFrontLightIsEnabled = sentFrontLightIsEnabled;
									resolve();
								},
								function onError() {
									console.log("Sending front light state -- FAIL");
									resolve();
								});
					} else {
						console.log("Not enough characteristics for sending light state");
						resolve();
					}
				});
		return frontLightStatePromise;
	};
	
	this.isDirty = function() {
		if(!this.lastUpdate) {
			return true;
		}
		return this.lastSentSpeed !== this.speed
			|| this.lastSentSteering !== this.steering
			|| this.lastSentTrunkIsOpen !== this.trunkIsOpen
			|| this.lastSentFrontLightIsEnabled !== this.frontLightIsEnabled;
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
		this.ensureConnected();
		this.isTransmitting = true;
		var instance = this;
		this.transmitSteeringPromise()
		.then(this.transmitSpeedPromise())
		.then(this.transmitTrunkStatePromise())
		.then(this.transmitFrontLightStatePromise())
		.then(function() {
			console.log("Transmitting finished");
			instance.lastUpdate = new Date();
		})
		.catch(function() {
		})
		.then(function() {
			instance.isTransmitting = false;
			instance.updateIfDirty();
		});
	};
}