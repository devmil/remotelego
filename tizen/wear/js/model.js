function CarModel (device) {
	this.SPEED_CHARACTERISTIC_UUID = "8d8ba32b-96be-4590-910b-c756c5222c9f";
	this.STEER_CHARACTERISTIC_UUID = "7baf8dca-2bfc-47fb-af29-042fccc180eb";
	this.device = device;
	this.service = undefined;
	
	this.lastSentSteering = undefined;
	this.lastSentSpeed = undefined;
	
	this.connect = function() {
/*		this.device.connect(
				function() {
					service = device.getService("40480f29-7bad-4ea5-8bf8-499405c9b324");
					isConnecting = false;
					isConnected = true;
				},
				function() {
					isConnecting = false;
					isConnected = false;
				});*/
		this.device.connect();
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
	};
	
	this.setSpeed = function(speedPercent) {
		if(this.lastSentSpeed == speedPercent) {
			return;
		}
		try {
			var data = new Array(speedPercent);

			this.getService().characteristics[0].writeValue(data);
			this.lastSentSpeed = speedPercent;
		} catch(ex) {
			console.log(ex);
		}
	};
	
	this.setSteering = function(steeringPercent) {
		if(this.lastSentSteering == steeringPercent) {
			return;
		}
		try {
			var positivePercent = (steeringPercent + 100) / 2;
			var data = new Array(positivePercent);
			
			this.getService().characteristics[1].writeValue(data);
			this.lastSentSteering = steeringPercent;
		}
		catch(ex) {
			console.log(ex);
		}
	};
}