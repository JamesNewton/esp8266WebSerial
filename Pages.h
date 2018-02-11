#ifndef PAGES_H
#define PAGES_H


/**** ADMIN MAIN PAGE ****/
const char PAGE_AdminMainPage[] PROGMEM = R"=====(
<!DOCTYPE html>
<HTML>
<HEAD>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
</HEAD>
<BODY>

<a href="/"  class="btn btn--s">&lt;</a>&nbsp;&nbsp;<strong>Administration</strong>
<hr>
<a href="general.html" style="width:250px" class="btn btn--m btn--blue" >General Configuration</a><br>
<a href="config.html" style="width:250px" class="btn btn--m btn--blue" >Network Configuration</a><br>
<a href="info.html"   style="width:250px"  class="btn btn--m btn--blue" >Network Information</a><br>
<a href="ntp.html"   style="width:250px"  class="btn btn--m btn--blue" >NTP Settings</a><br>


<script>
window.onload = function (){
	load("style.css","css", function() {
		load("microajax.js","js", function() {
				// Do something after load...
		});
	});
}
function load(e,t,n){
  if("js"==t){
      var a=document.createElement("script");
      a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)
      }
   else if("css"==t){
    var a=document.createElement("link");
    a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)
    }
  }
</script>
</BODY>
</HTML>
)=====";

/**** ADMIN GENERAL SETTINGS PAGE ****/
const char PAGE_AdminGeneralSettings[] PROGMEM =  R"=====(
<!DOCTYPE html>
<HTML>
<HEAD>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
</HEAD>
<BODY>
<a href="admin.html"  class="btn btn--s">&lt;</a>&nbsp;&nbsp;<strong>General Settings</strong>
<hr>
<form action="" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">Name of Device</td><td><input type="text" id="devicename" name="devicename" value=""></td></tr>
<tr><td align="right"> Enable Connection:</td><td><input type="checkbox" id="connect" name="connect"></td></tr>
<tr><td align="right"> Blink?:</td><td><div id="blinked">N/A</div></td></tr>
<tr><td align="left" colspan="2"><hr></td></tr>
<tr><td align="left" colspan="2">Logging</td></tr>
<tr><td align="right"> Enabled:</td><td><input type="checkbox" id="logging" name="logging"></td></tr>
<tr><td align="right"> Interval:</td><td><input type="text" id="interval" name="interval" size="6" value="0"> Seconds</td></tr>
<tr><td align="right"> Check in every:</td><td><input type="text" id="wakecount" name="wakecount" size="6" value="0"> intervals</td></tr>
<tr><td colspan="2">Stream Server:<br><input type="text" id="server" name="server" size="65" value=""></td></tr>
<tr><td colspan="2" align="center"><input disabled id="save" type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<br>Status <span id="status">&nbsp; </span>

<script>

var dataurl = "/data?text=";
var position = 0;
var scale = 50000;
var devstatus = document.getElementById('status');

function setValuesDone() {
  document.getElementById("save").disabled = false; 
  if ("Monster" == document.getElementById("devicename").value) {
    var elemDiv = document.createElement('div');
    elemDiv.style.cssText = 'position:absolute;width:100%;height:50px;opacity:0.3;z-index:100;background:#000;';
    elemDiv.addEventListener('click', clickSendNum);
    elemDiv.innterHTML='&nbsp;';
    document.body.appendChild(elemDiv);
    devstatus.addEventListener('click', function (e) {
      microAjax(dataurl+'?', function (res) {devstatus.innerText = res} ); 
      });
    devstatus.click();
    }
  }

function clickSendNum(e){
  var pos = {}, offset = {}, ref, x, y, w, move, cmd;
  devstatus.innerText = "Working, please wait...";
  var posdiv = e.target;//document.getElementsByClassName('position')[0];
  ref = posdiv.offsetParent;
  pos.x = !! e.touches ? e.touches[ 0 ].pageX : e.pageX;
  pos.y = !! e.touches ? e.touches[ 0 ].pageY : e.pageY;
  offset.left = posdiv.offsetLeft;
  offset.top = posdiv.offsetTop;
  while ( ref ) {
    offset.left += ref.offsetLeft;
    offset.top += ref.offsetTop;
    ref = ref.offsetParent;
    }
  x = pos.x - offset.left;
  y = pos.y - offset.top;
  w = posdiv.clientWidth;
  move = Math.floor((x/w)*scale) - position;
  position += move;
  cmd = dataurl+move+"%0D";
  //status.innerText = cmd;
  microAjax(cmd, function (res) {devstatus.innerText = res} ); 
  }
  
window.onload = function () {
  load("style.css","css", function() {
    load("microajax.js","js", function() {
      setValues("/admin/generalvalues");
      });
    });
  };
  
function load(e,t,n){
  if("js"==t){
      var a=document.createElement("script");
      a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)
      }
   else if("css"==t){
    var a=document.createElement("link");
    a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)
    }
  }
</script>
</BODY>
</HTML>
)=====";

void send_devicename_value_html(AsyncWebServerRequest *request) {		
	String values ="";
	values += "devicename|" + (String) config.DeviceName + "|div\n";
	request->send ( 200, "text/plain", values);
  debugbuf+=__FUNCTION__;
	}

void send_general_html(AsyncWebServerRequest *request){
	if (request->args() > 0 )  { // Save Settings	
		config.Logging = false;
    config.Connect = false; 
		//config.AutoTurnOff = false;
		String temp = "";
		for ( uint8_t i = 0; i < request->args(); i++ ) {
			if (request->argName(i) == "devicename") config.DeviceName = urldecode(request->arg(i)); 
			if (request->argName(i) == "logging") config.Logging = true; 
      if (request->argName(i) == "connect") config.Connect = true; 
      if (request->argName(i) == "interval") config.Interval =  request->arg(i).toInt(); 
      if (request->argName(i) == "wakecount") config.WakeCount =  request->arg(i).toInt(); 
      if (request->argName(i) == "server") config.streamServerURL = urldecode(request->arg(i)); 
		  }
		WriteConfig();
		firstStart = true;
    if (config.Connect) {
      digitalWrite(SERIAL_ENABLE_PIN, HIGH);
    } else {
      digitalWrite(SERIAL_ENABLE_PIN, LOW);
      }
    }
  if (config.Interval > 0) config.sleepy=true; else config.sleepy=false;
	request->send ( 200, "text/html", PAGE_AdminGeneralSettings ); 
	debugbuf+=__FUNCTION__;
	}

void send_general_configuration_values_html(AsyncWebServerRequest *request) {
	String values ="";
	values += "devicename|" + (String) config.DeviceName +  "|input\n";
  values += "interval|"   + (String) config.Interval +  "|input\n";
  values += "wakecount|"  + (String) config.WakeCount +  "|input\n";
	values += "logging|"    + (String) (config.Logging ? "checked" : "") + "|chk\n";
  values += "connect|"    + (String) (config.Connect ? "checked" : "") + "|chk\n";
  values += "server|"     + (String) config.streamServerURL + "|input\n";
  values += "blinked|"    + (String) (digitalRead(WAS_BLINK)? "YES": "NO") + "|div\n";
  digitalWrite(CLEAR_BLINK, LOW);
	request->send ( 200, "text/plain", values);
  debugbuf+=__FUNCTION__;
  digitalWrite(CLEAR_BLINK, HIGH);
  }
  
/**** ADMIN NETWORK INFORMATION PAGE ****/
const char PAGE_Information[] PROGMEM = R"=====(
<!DOCTYPE html>
<HTML>
<HEAD>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
</HEAD>
<BODY>
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script> 
<a href="admin.html"  class="btn btn--s">&lt;</a>&nbsp;&nbsp;<strong>Network Information</strong>
<hr>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">SSID :</td><td><span id="x_ssid"></span></td></tr>
<tr><td align="right">IP :</td><td><span id="x_ip"></span></td></tr>
<tr><td align="right">Netmask :</td><td><span id="x_netmask"></span></td></tr>
<tr><td align="right">Gateway :</td><td><span id="x_gateway"></span></td></tr>
<tr><td align="right">Mac :</td><td><span id="x_mac"></span></td></tr>

<tr><td colspan="2"><hr></span></td></tr>
<tr><td align="right">NTP Time:</td><td><span id="x_ntp"></span></td></tr>

<tr><td colspan="2" align="center"><a href="javascript:GetState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>
<script>

function GetState() {
	setValues("/admin/infovalues");
  }

window.onload = function () {
	load("style.css","css", function() {
		load("microajax.js","js", function() {
			GetState();
		  });
	  });
  }
function load(e,t,n){
  if("js"==t){
      var a=document.createElement("script");
      a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)
      }
   else if("css"==t){
    var a=document.createElement("link");
    a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)
    }
  }

</script>
</BODY>
</HTML>

)=====" ;


void send_information_values_html (AsyncWebServerRequest *request) {
  debugbuf+="/admin/infovalues.html"; 
	String values ="";
	values += "x_ssid|" + (String)WiFi.SSID() +  "|div\n";
	values += "x_ip|" +  (String) WiFi.localIP()[0] + "." +  (String) WiFi.localIP()[1] + "." +  (String) WiFi.localIP()[2] + "." + (String) WiFi.localIP()[3] +  "|div\n";
	values += "x_gateway|" +  (String) WiFi.gatewayIP()[0] + "." +  (String) WiFi.gatewayIP()[1] + "." +  (String) WiFi.gatewayIP()[2] + "." + (String) WiFi.gatewayIP()[3] +  "|div\n";
	values += "x_netmask|" +  (String) WiFi.subnetMask()[0] + "." +  (String) WiFi.subnetMask()[1] + "." +  (String) WiFi.subnetMask()[2] + "." + (String) WiFi.subnetMask()[3] +  "|div\n";
	values += "x_mac|" + GetMacAddress() +  "|div\n";
	values += "x_ntp|" +  (String) DateTime.hour + ":" + (String) + DateTime.minute +  ":"  + (String)  DateTime.second + " " + (String)   DateTime.year + "-" + (String)  DateTime.month + "-" + (String)  DateTime.day +  "|div\n";
	request->send ( 200, "text/plain", values);
  }


/**** ADMIN NETWORK INFORMATION PAGE ****/
const char PAGE_NetworkConfiguration[] PROGMEM = R"=====(
<!DOCTYPE html>
<HTML>
<HEAD>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
</HEAD>
<BODY>
<a href="admin.html"  class="btn btn--s">&lt;</a>&nbsp;&nbsp;<strong>Network Configuration</strong>
<hr>
Connect to Router with these settings:<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">SSID:</td><td><input type="text" id="ssid" name="ssid" value=""></td></tr>
<tr><td align="right">Password:</td><td><input type="text" id="password" name="password" value=""></td></tr>
<tr><td align="right">DHCP:</td><td><input type="checkbox" id="dhcp" name="dhcp"></td></tr>
<tr><td align="right">IP:     </td><td><input type="text" id="ip_0" name="ip_0" size="3">.<input type="text" id="ip_1" name="ip_1" size="3">.<input type="text" id="ip_2" name="ip_2" size="3">.<input type="text" id="ip_3" name="ip_3" value="" size="3"></td></tr>
<tr><td align="right">Netmask:</td><td><input type="text" id="nm_0" name="nm_0" size="3">.<input type="text" id="nm_1" name="nm_1" size="3">.<input type="text" id="nm_2" name="nm_2" size="3">.<input type="text" id="nm_3" name="nm_3" size="3"></td></tr>
<tr><td align="right">Gateway:</td><td><input type="text" id="gw_0" name="gw_0" size="3">.<input type="text" id="gw_1" name="gw_1" size="3">.<input type="text" id="gw_2" name="gw_2" size="3">.<input type="text" id="gw_3" name="gw_3" size="3"></td></tr>
<tr><td colspan="2" align="center"><input disabled id="save" type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<hr>
<strong>Connection State:</strong><div id="connectionstate">N/A</div>
<hr>
<strong>Networks:</strong><br>
<table border="0"  cellspacing="3" style="width:310px" >
<tr><td><div id="networks">Scanning...</div></td></tr>
<tr><td align="center"><a href="javascript:GetState()" style="width:150px" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>

<script>

function GetState() {
  document.getElementById("networks").innerHTML = "Scanning...";
	setValues("/admin/connectionstate");
  }
function selssid(value){
	document.getElementById("ssid").value = value; 
  }

window.onload = function () {
  
	load("style.css","css", function() {
		load("microajax.js","js", function() {
  	  setValues("/admin/values");
      document.getElementById("save").disabled = false; 
			setTimeout(GetState,3000);
		  });
	  });
  }
  
function load(e,t,n){
  if("js"==t){
      var a=document.createElement("script");
      a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)
      }
   else if("css"==t){
    var a=document.createElement("link");
    a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)
    }
  }
</script>
</BODY>
</HTML>

)=====";

const char PAGE_WaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=config.html">
Please Wait....Configuring and Restarting.
)=====";


//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
void send_network_configuration_html(AsyncWebServerRequest *request) {
	
	if (request->args() > 0 ) {  // Save Settings
		String temp = "";
		config.dhcp = false;
		for ( uint8_t i = 0; i < request->args(); i++ ) {
			if (request->argName(i) == "ssid") config.ssid =   urldecode(request->arg(i));
			if (request->argName(i) == "password") config.password =    urldecode(request->arg(i)); 
			if (request->argName(i) == "ip_0") if (checkRange(request->arg(i))) 	config.IP[0] =  request->arg(i).toInt();
			if (request->argName(i) == "ip_1") if (checkRange(request->arg(i))) 	config.IP[1] =  request->arg(i).toInt();
			if (request->argName(i) == "ip_2") if (checkRange(request->arg(i))) 	config.IP[2] =  request->arg(i).toInt();
			if (request->argName(i) == "ip_3") if (checkRange(request->arg(i))) 	config.IP[3] =  request->arg(i).toInt();
			if (request->argName(i) == "nm_0") if (checkRange(request->arg(i))) 	config.Netmask[0] =  request->arg(i).toInt();
			if (request->argName(i) == "nm_1") if (checkRange(request->arg(i))) 	config.Netmask[1] =  request->arg(i).toInt();
			if (request->argName(i) == "nm_2") if (checkRange(request->arg(i))) 	config.Netmask[2] =  request->arg(i).toInt();
			if (request->argName(i) == "nm_3") if (checkRange(request->arg(i))) 	config.Netmask[3] =  request->arg(i).toInt();
			if (request->argName(i) == "gw_0") if (checkRange(request->arg(i))) 	config.Gateway[0] =  request->arg(i).toInt();
			if (request->argName(i) == "gw_1") if (checkRange(request->arg(i))) 	config.Gateway[1] =  request->arg(i).toInt();
			if (request->argName(i) == "gw_2") if (checkRange(request->arg(i))) 	config.Gateway[2] =  request->arg(i).toInt();
			if (request->argName(i) == "gw_3") if (checkRange(request->arg(i))) 	config.Gateway[3] =  request->arg(i).toInt();
			if (request->argName(i) == "dhcp") config.dhcp = true;
        		}
		 request->send ( 200, "text/html", PAGE_WaitAndReload );
		WriteConfig();
		ConfigureWifi();
		AdminTimeOutCounter=0;
        	}
	else {
		request->send ( 200, "text/html", PAGE_NetworkConfiguration ); 
	   }
	debugbuf+=__FUNCTION__; 
    }


//   FILL THE PAGE WITH VALUES
void send_network_configuration_values_html(AsyncWebServerRequest *request) {
	String values ="";

	values += "ssid|" + (String) config.ssid + "|input\n";
	values += "password|" +  (String) config.password + "|input\n";
	values += "ip_0|" +  (String) config.IP[0] + "|input\n";
	values += "ip_1|" +  (String) config.IP[1] + "|input\n";
	values += "ip_2|" +  (String) config.IP[2] + "|input\n";
	values += "ip_3|" +  (String) config.IP[3] + "|input\n";
	values += "nm_0|" +  (String) config.Netmask[0] + "|input\n";
	values += "nm_1|" +  (String) config.Netmask[1] + "|input\n";
	values += "nm_2|" +  (String) config.Netmask[2] + "|input\n";
	values += "nm_3|" +  (String) config.Netmask[3] + "|input\n";
	values += "gw_0|" +  (String) config.Gateway[0] + "|input\n";
	values += "gw_1|" +  (String) config.Gateway[1] + "|input\n";
	values += "gw_2|" +  (String) config.Gateway[2] + "|input\n";
	values += "gw_3|" +  (String) config.Gateway[3] + "|input\n";
	values += "dhcp|" +  (String) (config.dhcp ? "checked" : "") + "|chk\n";
	request->send ( 200, "text/plain", values);
	debugbuf+=__FUNCTION__; 
       }


//   FILL THE PAGE WITH NETWORKSTATE & NETWORKS
void send_connection_state_values_html(AsyncWebServerRequest *request) {
	String state = get_wifi_status();
	String Networks = "";
	 int n = WiFi.scanNetworks();
 
	 if (n == 0) {
		 Networks = "<font color='#FF0000'>No networks found!</font>";
	        }
	else {
		Networks = "Found " +String(n) + " Networks<br>";
		Networks += "<table border='0' cellspacing='0' cellpadding='3'>";
		Networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
		for (int i = 0; i < n; ++i) {
			int quality=0;
			if(WiFi.RSSI(i) <= -100) { quality = 0; }
			else if(WiFi.RSSI(i) >= -50) { quality = 100; }
			else { quality = 2 * (WiFi.RSSI(i) + 100);}
			Networks += "<tr><td><a href='javascript:selssid(\""  +  String(WiFi.SSID(i))  + "\")'>"  +  String(WiFi.SSID(i))  + "</a></td><td>" +  String(quality) + "%</td><td>" +  String((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*")  + "</td></tr>";
		        }
		Networks += "</table>";
        	}
	String values ="";
	values += "connectionstate|" +  state +  "|div\n";
	values += "networks|" +  Networks + "|div\n";
	request->send ( 200, "text/plain", values);
  debugbuf+=__FUNCTION__; 
        }


/**** ADMIN NETWORK TIME SETTINGS PAGE ****/
const char PAGE_NTPConfiguration[] PROGMEM = R"=====(
<!DOCTYPE html>
<HTML>
<HEAD>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
</HEAD>
<BODY>
<a href="admin.html"  class="btn btn--s">&lt;</a>&nbsp;&nbsp;<strong>NTP Settings</strong>
<hr>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">NTP Server:</td><td><input type="text" id="ntpserver" name="ntpserver" maxlength="172" value=""></td></tr>
<tr><td align="right">Update:</td><td><input type="text" id="update" name="update" size="3"maxlength="6" value=""> minutes (0=disable)</td></tr>
<tr><td>Timezone</td><td>
<select  id="tz" name="tz">
	<option value="-120">(GMT-12:00)</option>
	<option value="-110">(GMT-11:00)</option>
	<option value="-100">(GMT-10:00)</option>
	<option value="-90">(GMT-09:00)</option>
	<option value="-80">(GMT-08:00)</option>
	<option value="-70">(GMT-07:00)</option>
	<option value="-60">(GMT-06:00)</option>
	<option value="-50">(GMT-05:00)</option>
	<option value="-40">(GMT-04:00)</option>
	<option value="-35">(GMT-03:30)</option>
	<option value="-30">(GMT-03:00)</option>
	<option value="-20">(GMT-02:00)</option>
	<option value="-10">(GMT-01:00)</option>
	<option value="0">(GMT+00:00)</option>
	<option value="10">(GMT+01:00)</option>
	<option value="20">(GMT+02:00)</option>
	<option value="30">(GMT+03:00)</option>
	<option value="35">(GMT+03:30)</option>
	<option value="40">(GMT+04:00)</option>
	<option value="45">(GMT+04:30)</option>
	<option value="50">(GMT+05:00)</option>
	<option value="55">(GMT+05:30)</option>
	<option value="57">(GMT+05:45)</option>
	<option value="60">(GMT+06:00)</option>
	<option value="65">(GMT+06:30)</option>
	<option value="70">(GMT+07:00)</option>
	<option value="80">(GMT+08:00)</option>
	<option value="90">(GMT+09:00)</option>
	<option value="95">(GMT+09:30)</option>
	<option value="100">(GMT+10:00)</option>
	<option value="110">(GMT+11:00)</option>
	<option value="120">(GMT+12:00)</option>
	<option value="120">(GMT+12:00)</option>
	<option value="130">(GMT+13:00)</option>
</select>
</td></tr>
<tr><td align="right">Daylight saving:</td><td><input type="checkbox" id="dst" name="dst"></td></tr>
<tr><td colspan="2" align="center"><input disabled id="save" type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>

<script>

window.onload = function () {
  load("style.css","css", function() {
    load("microajax.js","js", function() {
      setValues("/admin/ntpvalues");
      document.getElementById("save").disabled = false; 
      });
    });
  }
  
function load(e,t,n){
  if("js"==t){
      var a=document.createElement("script");
      a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)
      }
   else if("css"==t){
    var a=document.createElement("link");
    a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)
    }
  }

</script>
</BODY>
</HTML>
)=====";

void send_NTP_configuration_html(AsyncWebServerRequest *request) {
	if (request->args() > 0 ) { // Save Settings
		config.daylight = false;
		String temp = "";
		for ( uint8_t i = 0; i < request->args(); i++ ) {
			if (request->argName(i) == "ntpserver") config.ntpServerName = urldecode( request->arg(i)); 
			if (request->argName(i) == "update") config.Update_Time_Via_NTP_Every =  request->arg(i).toInt(); 
			if (request->argName(i) == "tz") config.timezone =  request->arg(i).toInt(); 
			if (request->argName(i) == "dst") config.daylight = true; 
      }
		WriteConfig();
		firstStart = true;
    }
	request->send ( 200, "text/html", PAGE_NTPConfiguration ); 
  debugbuf+=__FUNCTION__; 
	}

void send_NTP_configuration_values_html(AsyncWebServerRequest *request){		
	String values ="";
	values += "ntpserver|" + (String) config.ntpServerName + "|input\n";
	values += "update|" +  (String) config.Update_Time_Via_NTP_Every + "|input\n";
	values += "tz|" +  (String) config.timezone + "|input\n";
	values += "dst|" +  (String) (config.daylight ? "checked" : "") + "|chk\n";
	request->send ( 200, "text/plain", values);
  debugbuf+=__FUNCTION__; 
	}


#endif
