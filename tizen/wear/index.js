var models = {};

( function () {
	var snapListviewWidget, list;
	var page = document.getElementById("main");
	var devices = [];
	var adapter;
	
	page.addEventListener("pagebeforeshow", function pageShowHandler(e) {
		list = document.querySelector(".ui-listview");
		if (list) {
			snapListviewWidget = tau.widget.SnapListview(list); // for call refresh()
		}
		
		if(tizen.bluetooth) {
			adapter = tizen.bluetooth.getLEAdapter();
		} else {
			adapter = undefined;
		}
		
		scan();
	}, false);
	
	page.addEventListener("pagebeforehide", function pageHideHandler(e) {
		stopScan();
	}, false);
	
	
	function addDevice(device) {
		for(var i=0; i<devices.length; i++) {
			if(devices[i].address === device.address) {
				return false;
			}
		}
		devices.push({ name: device.name, address: device.address });
		var listItem = document.createElement("li"); // make li element
		listItem.innerHTML = "<a href=\"control.html?address=" + device.address + "\">" + device.name + "</a>";
		list.appendChild(listItem);                 // append it to list
		snapListviewWidget.refresh();               // important! please call refresh() of SnaplistView
		
		var model = new CarModel(device);
		models[device.address] = model;
		
		return true;
	}
	
	function onScanError(error) {
		console.log("Scan error: " + error);
	}
	
	function onDeviceFound(device)
	{
		if(device.uuids.length > 0 && device.uuids[0].toLowerCase() === "40480f29-7bad-4ea5-8bf8-499405c9b324") {
			addDevice(device);
		}
	}
	
	function scan() {
		console.log("Start scan");
		if(adapter) {
			adapter.startScan(onDeviceFound, onScanError);
		} else {
			var dummyDevice = new Object();
			dummyDevice.name = "Dummy";
			dummyDevice.address = "NO:NE";
			addDevice(dummyDevice);
		}
	}
	
	function stopScan() {
		if(adapter) {
			adapter.stopScan();
		}
	}
} () );
