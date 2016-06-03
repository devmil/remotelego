( function () {
	var page = document.getElementById("control");
	var directionControl;
	var directionControlKnob;
	var lastTouch = null;
	
	var carModel;
	
	page.addEventListener("pagebeforeshow", function pageShowHandler(e) {
		directionControl = document.querySelector("#directioncontrol");
		directionControlKnob = document.querySelector("#directioncontrol-knob");
		
		directionControl.addEventListener("touchstart", onDirectionControlTouchStart);
		directionControl.addEventListener("touchend", 	onDirectionControlTouchEnd);
		directionControl.addEventListener("touchmove", 	onDirectionControlTouchMove);
		
		//alert("l:" + directionControlKnob.offsetLeft); //0
		//alert("t:" + directionControlKnob.offsetTop); //115
		
		carModel = models[getParameterByName("address")];
		carModel.connect();
		
		setKnobPosFromSpeedAndSteer(0, 0);
	}, false);
	
	page.addEventListener("pagebeforehide", function pageBeforeHideHandler(e) {
		if(carModel) {
			carModel.disconnect();
		}
	}, false);
	
	function getParameterByName(name) {
	    var url = window.location.href;
	    name = name.replace(/[\[\]]/g, "\\$&");
	    var regex = new RegExp("[?&]" + name + "(=([^&#]*)|&|#|$)"),
	        results = regex.exec(url);
	    if (!results) return null;
	    if (!results[2]) return '';
	    return decodeURIComponent(results[2].replace(/\+/g, " "));
	}
	
	function onDirectionControlTouchStart(ev) {
		ev.preventDefault();
		lastTouch = ev.changedTouches[0];
	    window.requestAnimationFrame(handleTouch);
	}
	
	function onDirectionControlTouchEnd(ev) {
		ev.preventDefault();
		setKnobPosFromSpeedAndSteer(0, 0);
	}
	
	function onDirectionControlTouchMove(ev) {
		ev.preventDefault();
		lastTouch = ev.changedTouches[0];
	    window.requestAnimationFrame(handleTouch);
	}
	
	function handleTouch(){
		setKnobPosFromTouch(lastTouch);
	}
	
	function setKnobPosFromSpeedAndSteer(speedPercent, steerPercent) {
		var availableWidth = directionControl.offsetWidth - directionControlKnob.offsetWidth;
		var availableHeight = directionControl.offsetHeight - directionControlKnob.offsetHeight;
		
		var originOffsetX = directionControl.offsetLeft + directionControlKnob.offsetWidth / 2;
		var originOffsetY = directionControl.offsetTop + directionControlKnob.offsetHeight / 2;

		var offsetX = originOffsetX + ((steerPercent + 100) * availableWidth) / 200; 
		var offsetY = originOffsetY + ((speedPercent + 100) * availableHeight) / 200; 

		directionControlKnob.style.transform = 
			"translate3d(" + (offsetX - originOffsetX) + "px," + (offsetY - originOffsetY) + "px," + "0px)";
		
		carModel.setSteering(steerPercent);
		carModel.setSpeed(speedPercent);
	}
	
	function setKnobPosFromTouch(touch) {
		var availableWidth = directionControl.offsetWidth - directionControlKnob.offsetWidth;
		var availableHeight = directionControl.offsetHeight - directionControlKnob.offsetHeight;
		
		var originOffsetX = directionControl.offsetLeft + directionControlKnob.offsetWidth / 2;
		var originOffsetY = directionControl.offsetTop + directionControlKnob.offsetHeight / 2;
		
		var translateX = touch.pageX - originOffsetX;
		var translateY = touch.pageY - originOffsetY;
		
		if(translateX < 0
				|| translateY < 0
				|| translateX > availableWidth
				|| translateY > availableHeight) {
			return;
		}
		
		directionControlKnob.style.transform = 
			"translate3d(" + (translateX) + "px," + (translateY) + "px," + "0px)";
		
		var xPercent = ((touch.pageX - originOffsetX) * 100) / availableWidth;
		var yPercent = ((touch.pageY - originOffsetY) * 100) / availableHeight;
		
		var steer = (xPercent * 2) - 100;
		var speed = (yPercent * 2) - 100;
		
		carModel.setSteering(steer);
		carModel.setSpeed(speed);
	}
	
} () );
