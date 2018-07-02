#ifndef PAGES_H
#define PAGES_H


String send_tag_values(const String& tag) {
  if (tag == "devicename") return (String) config.DeviceName;
  if (tag == "baud") return (String) config.baud;
  if (tag == "Connect") return (String) (config.Connect ? "checked" : "");
  if (tag == "logging") return (String) (config.Logging ? "checked" : "");
  if (tag == "server") return (String) config.streamServerURL;
  if (tag == "interval") return (String) config.Interval;
  if (tag == "wakecount") return (String) config.WakeCount;
  if (tag == "blinked") return (String) (digitalRead(WAS_BLINK)? "YES": "NO");
  if (tag == "datatrigger") return (String) config.datatrigger;
  if (tag == "dataregexp1") return (String) config.dataregexp1;
  if (tag == "dataslope1") return String(config.dataslope1,7);
  if (tag == "dataoffset1") return String(config.dataoffset1,7);
  if (tag == "dataname1") return (String) config.dataname1;
  if (tag == "dataslope2") return String(config.dataslope2,7);
  if (tag == "dataoffset2") return String(config.dataoffset2,7);
  if (tag == "dataname2") return (String) config.dataname2;
  if (tag == "dataslope3") return String(config.dataslope3,7);
  if (tag == "dataoffset3") return String(config.dataoffset3,7);
  if (tag == "dataname3") return (String) config.dataname3;
  if (tag == "datacount") return (String) config.datacount;
  if (tag == "pwronstr") return (String) config.pwronstr;
  if (tag == "pwrondelay") return (String) config.pwrondelay;

  
  if (tag == "directory") {
    String dirlist = "";
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      dirlist += "<a href=\"/update?delete=";
      dirlist += String(dir.fileName());
      dirlist += "\">x</a> ";
      dirlist += String(dir.fileName());
      dirlist += "\t ";
      File f = dir.openFile("r");
      dirlist += String(f.size());
      dirlist += " bytes\n ";
      }
    return dirlist ;
    }
}

/**** ADMIN MAIN PAGE ****/
const char PAGE_AdminMainPage[] PROGMEM = R"=====(
<!DOCTYPE html>
<HTML>
<HEAD>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="/style.css" />

</HEAD>
<BODY>

<a href="/"  class="btn btn--s">&lt;</a>&nbsp;&nbsp;<strong>Administration</strong>
<hr>
<a href="general.html" style="width:250px" class="btn btn--m btn--blue" >General Configuration</a><br>
<a href="config.html" style="width:250px" class="btn btn--m btn--blue" >Network Configuration</a><br>
<a href="info.html"   style="width:250px"  class="btn btn--m btn--blue" >Network Information</a><br>
<a href="ntp.html"   style="width:250px"  class="btn btn--m btn--blue" >NTP Settings</a><br>
<a href="device.html"   style="width:250px"  class="btn btn--m btn--blue" >Connected Device Setup</a><br>
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
<link rel="stylesheet" href="/style.css" />
<script src="microajax.js"></script> 
</HEAD>
<BODY>
<a href="admin.html"  class="btn btn--s">&lt;</a>&nbsp;&nbsp;<strong>General Settings</strong>
<hr>
<form action="" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">Name of Device</td><td><input type="text" id="devicename" name="devicename" value="`devicename`"></td></tr>
<tr><td align="right"> Enable Connection:</td><td><input type="checkbox" id="connect" name="connect" `Connect`></td></tr>
<tr><td align="right"> Blink?:</td><td><div id="blinked">`blinked`</div></td></tr>
<tr><td align="left" colspan="2"><hr></td></tr>
<tr><td align="left" colspan="2">Logging</td></tr>
<tr><td align="right"> Enabled:</td><td><input type="checkbox" id="logging" name="logging" `logging`></td></tr>
<tr><td align="right"> Interval:</td><td><input type="text" id="interval" name="interval" size="6" value="`interval`"> Seconds</td></tr>
<tr><td align="right"> Check in every:</td><td><input type="text" id="wakecount" name="wakecount" size="6" value="`wakecount`"> intervals</td></tr>
<tr><td colspan="2">Server:<br><input type="text" id="server" name="server" size="65" value="`server`"></td></tr>
<tr><td colspan="2" align="center"><input id="save" type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<br>Status <span id="status">&nbsp; </span>

<script>

var dataurl = "/data?text=";
var position = 0;
var scale = 50000;
var devstatus = document.getElementById('status');

function setValuesDone() {
   
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
  setValuesDone();
  };
  
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
			if (request->argName(i) == "devicename") urldecode(request->arg(i), config.DeviceName, sizeof(config.DeviceName)); 
			if (request->argName(i) == "logging") config.Logging = true; 
      if (request->argName(i) == "connect") config.Connect = true; 
      if (request->argName(i) == "interval") config.Interval =  request->arg(i).toInt(); 
      if (request->argName(i) == "wakecount") config.WakeCount =  request->arg(i).toInt(); 
      if (request->argName(i) == "server") urldecode(request->arg(i), config.streamServerURL, sizeof(config.streamServerURL)); 
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
	request->send_P ( 200, "text/html", PAGE_AdminGeneralSettings, send_tag_values ); 
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
  pinMode(CLEAR_BLINK, OUTPUT); //incase it was overwritten by Serial1 NeoPixel cmdString
	request->send ( 200, "text/plain", values);
  debugbuf+=__FUNCTION__;
  digitalWrite(CLEAR_BLINK, HIGH);
  pinMode(CLEAR_BLINK, OUTPUT); //incase it was overwritten by Serial1 NeoPixel cmdString
  }

/**** FILE MANAGER PAGE ****/

const char PAGE_FileSystem[] PROGMEM =  R"=====(
<!DOCTYPE html>
<HTML>
<HEAD>
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <link rel="stylesheet" href="/style.css">
  </HEAD>
<BODY>
  <a href="admin.html"  class="btn btn--s">&lt;</a>&nbsp;&nbsp;<strong>File System</strong>
  <hr>
  <pre> `directory`</pre>
    <form method="post" enctype="multipart/form-data" action="/update">
      Upload: <br><input type="file" name="name">
      <input class="button" type="submit" value="Upload">
      </form>
   Delete: Press 'x' before file, then append '&now' to url to actually remove the file. 
  </BODY>
</HTML>
)=====";

void handle_fs_upload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
static File fsUploadFile; //Used with SPIFFS to upload files. 
//Needs to persist because multiple calls are made to write data.

  if(!index){
    fsUploadFile = SPIFFS.open("/"+filename, "w"); 
    //opening forward slash is required, or SPIFFS doesn't see the file as being in the root "folder"
    debugbuf+="Upload:";
    debugbuf+=filename;
    debugbuf+=" handle ";
    debugbuf+=String(fsUploadFile);
    debugbuf+="\n";
  }
  if (len) {
    fsUploadFile.write(data, len);
    debugbuf+=String((len));
    debugbuf+=" bytes\n";
    }
  if(final){
    if(fsUploadFile) fsUploadFile.close(); 
    debugbuf+=" done\n";
  }
  //yield(); //https://github.com/esp8266/Arduino/issues/1045
}

void send_fs_html(AsyncWebServerRequest *request){
  if (request->args() > 0 )  { // Work to do
    for ( uint8_t i = 0; i < request->args(); i++ ) {
      if (request->argName(i) == "delete") {
        debugbuf+= "delete:" + request->arg(i)+".\n"; 
        if (request->hasArg("now")) SPIFFS.remove(request->arg(i));
        }
      }
    }
  debugbuf+=__FUNCTION__;
  request->send_P ( 200, "text/html", PAGE_FileSystem, send_tag_values ); 
  }

/**** CONNECTED DEVICE SETUP PAGE ****/

const char PAGE_AdminDeviceSettings[] PROGMEM =  R"=====(
<!DOCTYPE html>
<HTML>
<HEAD>
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <link rel="stylesheet" href="/style.css">
  </HEAD>
<BODY>
  <a href="admin.html"  class="btn btn--s">&lt;</a>&nbsp;&nbsp;<strong>Connected Device Settings</strong>
  <hr>
  <form action="" method="post">
    <table border="0"  cellspacing="0" cellpadding="3" >
      <tr><td align="right">Baud rate</td><td><input type="text" id="baud" name="baud" value="`baud`"></td></tr>
      <tr><td align="right"> Connection:</td><td><input type="checkbox" id="connect" name="connect" `Connect`>Enabled</input></td></tr>
      <tr><td align="right"> Blink?:</td><td><div id="blinked">`blinked`</div></td></tr>
      <tr><td align="left" colspan="2"><hr></td></tr>
      <tr><td align="right"> Initialization:</td><td>
        Send: <input type="text" id="pwronstr" name="pwronstr" value="`pwronstr`" size=6>
        after <input type="text" id="pwrondelay" name="pwrondelay" value="`pwrondelay`" size=3 title="zero to disable"> Seconds
        </td></tr>
      <tr><td align="left" colspan="2"><hr></td></tr>
      <tr><td align="left" colspan="2">Extracted Data Reading: (<a href="http://www.cplusplus.com/reference/cstdio/scanf/">Data Pattern help</a>)</td></tr>
      <tr><td align="right"> Trigger:</td><td>
        <input type="text" id="datatrigger" name="datatrigger" value="`datatrigger`" size=6>
        Every <input type="text" id="datatcount" name="datacount" value="`datacount`" size=3> Seconds
        </td></tr>
      <tr><td align="right"> Data Pattern:</td><td><input type="text" id="dataregexp1" name="dataregexp1" value="`dataregexp1`"></td></tr>
      <tr><td align="right"> Name:</td>
        <td>
          <input type="text" id="dataname1" name="dataname1" size="6" value="`dataname1`">
          <input type="text" id="dataname2" name="dataname2" size="6" value="`dataname2`">
          <input type="text" id="dataname3" name="dataname3" size="6" value="`dataname3`">
          </td>
        </tr>      <tr><td align="right"> Slope:</td>
        <td>
          <input type="text" id="dataslope1" name="dataslope1" size="6" value="`dataslope1`">
          <input type="text" id="dataslope2" name="dataslope2" size="6" value="`dataslope2`">
          <input type="text" id="dataslope3" name="dataslope3" size="6" value="`dataslope3`">
          </td>
        </tr>
      <tr><td align="right"> Offset:</td>
        <td>
          <input type="text" id="dataoffset1" name="dataoffset1" size="6" value="`dataoffset1`">
          <input type="text" id="dataoffset2" name="dataoffset2" size="6" value="`dataoffset2`">
          <input type="text" id="dataoffset3" name="dataoffset3" size="6" value="`dataoffset3`">
          </td>
        </tr>
      <tr><td colspan="2" align="center">
        <input id="save"  type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save">
        </td></tr>
      </table>
    </form>
  </BODY>
</HTML>
)=====";


void send_device_html(AsyncWebServerRequest *request){
  if (request->args() > 0 )  { // Save Settings 
    config.Connect = false; //guess
    for ( uint8_t i = 0; i < request->args(); i++ ) {
      if (request->argName(i) == "baud") config.baud = request->arg(i).toInt(); 
      else if (request->argName(i) == "connect") config.Connect = true; 
      else if (request->argName(i) == "datatrigger") strlcpy( config.datatrigger, HTMLencode(request->arg(i).c_str()).c_str(), sizeof(config.datatrigger)); 
      else if (request->argName(i) == "dataregexp1") strlcpy( config.dataregexp1, request->arg(i).c_str(), sizeof(config.dataregexp1)); 
      else if (request->argName(i) == "dataslope1") config.dataslope1 = request->arg(i).toFloat(); 
      else if (request->argName(i) == "dataoffset1") config.dataoffset1 = request->arg(i).toFloat(); 
      else if (request->argName(i) == "dataname1") strlcpy( config.dataname1, request->arg(i).c_str(), sizeof(config.dataname1)); 
      else if (request->argName(i) == "dataslope2") config.dataslope2 = request->arg(i).toFloat(); 
      else if (request->argName(i) == "dataoffset2") config.dataoffset2 = request->arg(i).toFloat(); 
      else if (request->argName(i) == "dataname2") strlcpy( config.dataname2, request->arg(i).c_str(), sizeof(config.dataname2)); 
      else if (request->argName(i) == "dataslope3") config.dataslope3 = request->arg(i).toFloat(); 
      else if (request->argName(i) == "dataoffset3") config.dataoffset3 = request->arg(i).toFloat(); 
      else if (request->argName(i) == "dataname3") strlcpy( config.dataname3, request->arg(i).c_str(), sizeof(config.dataname3)); 
      else if (request->argName(i) == "datacount") config.datacount = request->arg(i).toInt(); 
      else if (request->argName(i) == "pwronstr") strlcpy( config.pwronstr, request->arg(i).c_str(), sizeof(config.pwronstr)); 
      else if (request->argName(i) == "pwrondelay") config.pwrondelay = request->arg(i).toInt(); 
      }
    WriteConfig();
    if (config.Connect) {
        digitalWrite(SERIAL_ENABLE_PIN, HIGH);
      } else {
        digitalWrite(SERIAL_ENABLE_PIN, LOW);
        }
    }
  request->send_P ( 200, "text/html", PAGE_AdminDeviceSettings, send_tag_values ); 
  debugbuf+=__FUNCTION__;
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
			if (request->argName(i) == "ssid") urldecode(request->arg(i), config.ssid, sizeof(config.ssid));
			if (request->argName(i) == "password") urldecode(request->arg(i), config.password, sizeof(config.password)); 
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
			if (request->argName(i) == "ntpserver") urldecode( request->arg(i), config.ntpServerName, sizeof(config.ntpServerName)); 
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
