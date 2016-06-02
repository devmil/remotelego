( function () {
	var page = document.getElementById("control");
	var directionControl;
	
	debugger;
	
	page.addEventListener("pagebeforeshow", function pageShowHandler(e) {
		debugger;
		directionControl = document.getElementById("directioncontrol");
		
		directionControl.addEventListener("touchstart", onDirectionControlTouchStart);
		directionControl.addEventListener("touchend", 	onDirectionControlTouchEnd);
		directionControl.addEventListener("touchmove", 	onDirectionControlTouchMove);
	}, false);
	
	function onDirectionControlTouchStart(ev) {
		debugger;
		console.log(ev);
	}
	
	function onDirectionControlTouchEnd(ev) {
		debugger;
		console.log(ev);
	}
	
	function onDirectionControlTouchMove(ev) {
		debugger;
		console.log(ev);
	}
	
} () );
