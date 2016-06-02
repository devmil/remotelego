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
		
		adapter = tizen.bluetooth.getLEAdapter();

		scan();
	}, false);
	
	page.addEventListener("pagebeforehide", function pageHideHandler(e) {
		stopScan();
	}, false);
	
	
	function addDevice(name, address) {
		for(var i=0; i<devices.length; i++) {
			if(devices[i].address === address) {
				return;
			}
		}
		devices.push({ name: name, address: address });
		var listItem = document.createElement("li"); // make li element
		listItem.innerHTML = "<a href=\"control.html?address=" + address + "\">" + name + "</a>";
		list.appendChild(listItem);                 // append it to list
		snapListviewWidget.refresh();               // important! please call refresh() of SnaplistView
	}
	
	function onScanError(error) {
		console.log("Scan error: " + error);
	}
	
	function onDeviceFound(device)
	{
		if(device.uuids.length > 0 && device.uuids[0].toLowerCase() === "40480f29-7bad-4ea5-8bf8-499405c9b324") {
			addDevice(device.name, device.address);
		}
	}
	
	function scan() {
		console.log("Start scan");
		adapter.startScan(onDeviceFound, onScanError);
	}
	
	function stopScan() {
		if(adapter) {
			adapter.stopScan();
		}
	}
} () );
