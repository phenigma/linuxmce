var countries_url="http://10.0.2.5/pluto-lite/server/wtimezone.php";
var regions_url="http://10.0.2.5/pluto-lite/server/wtimezone.php?countryID=";
var cities_url="http://10.0.2.5/pluto-lite/server/wtimezone.php?regionID=";
var timezones_url="http://10.0.2.5/pluto-lite/server/wtimezone.php?timezones";

function makeHttpRequest(url, callback_function)
{
   var http_request = false;

   if (window.XMLHttpRequest) { // Mozilla, Safari,...
       http_request = new XMLHttpRequest();
   } else if (window.ActiveXObject) { // IE
       try {
           http_request = new ActiveXObject("Msxml2.XMLHTTP");
       } catch (e) {
           try {
               http_request = new ActiveXObject("Microsoft.XMLHTTP");
           } catch (e) {}
       }
   }

   if (!http_request) {
       alert('Unfortunatelly you browser doesn\'t support this feature.');
       return false;
   }
   http_request.onreadystatechange = function() {
       if (http_request.readyState == 4) {
           if (http_request.status == 200) {
               eval(callback_function + '(http_request.responseText)');
           } else {
               alert('There was a problem with the request.(Code: ' + http_request.status + ')');
           }
       }
   }
   http_request.open('GET', url, true);
   http_request.send(null);
}

function populate_countries(JSON_text){
	var countries = eval("(" + JSON_text + ")");
	ClearOptions(document.getElementById('country'));
	for(var countryID in countries){
		AddToOptionList(document.getElementById('country'), countryID, countries[countryID]);
	}
}

function populate_zones(JSON_text){
	var timezones = eval("(" + JSON_text + ")");
	ClearOptions(document.getElementById('timezone'));
	for(var timezoneID in timezones){
		AddToOptionList(document.getElementById('timezone'), timezoneID, timezones[timezoneID]);
	}
}


function prepare_data(){
	makeHttpRequest(countries_url, 'populate_countries');	
	makeHttpRequest(timezones_url, 'populate_zones');	
}

function ClearOptions(OptionList) {

   // Always clear an option list from the last entry to the first
   for (x = OptionList.length; x >= 0; x--) {
      OptionList[x] = null;
   }
}


function AddToOptionList(OptionList, OptionValue, OptionText) {
   // Add option to the bottom of the list
   OptionList[OptionList.length] = new Option(OptionText, OptionValue);
}

function prepare_region(){
	countryID=document.getElementById("country").value;
	if(countryID!=0){
		makeHttpRequest(regions_url+countryID, 'populate_region');	
	}
}

function populate_region(JSON_text){
	var regions = eval("(" + JSON_text + ")");
	ClearOptions(document.getElementById('region'));

	for(var regionID in regions){
		AddToOptionList(document.getElementById('region'), regionID, regions[regionID]);
	}
}

function prepare_city(){
	regionID=document.getElementById("region").value;
	if(regionID!=0){
		makeHttpRequest(cities_url+regionID, 'populate_city');	
	}
}

function populate_city(JSON_text){
	var cities = eval("(" + JSON_text + ")");
	ClearOptions(document.getElementById('city'));

	for(var timezoneID in cities){
		AddToOptionList(document.getElementById('city'), timezoneID, cities[timezoneID]);
	}
}

function set_timezone(){
	cityTimezone=document.getElementById("city").value;	
	if(cityTimezone!=0){
		var pos=0;
		for(i=0;i<document.getElementById("timezone").length;i++){
			if(document.getElementById("timezone").options[i]==cityTimezone){
				pos=i;
			}
		}
		alert(pos);
	}
}
