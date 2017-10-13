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


// Functions for this Page
void send_devicename_value_html() {		
	String values ="";
	values += "devicename|" + (String) config.DeviceName + "|div\n";
	server.send ( 200, "text/plain", values);
	debugln("",__FUNCTION__); 
	}

void send_general_html(){
	if (server.args() > 0 )  { // Save Settings	
		config.Logging = false;
    config.Connect = false; 
		//config.AutoTurnOff = false;
		String temp = "";
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			if (server.argName(i) == "devicename") config.DeviceName = urldecode(server.arg(i)); 
			if (server.argName(i) == "logging") config.Logging = true; 
      if (server.argName(i) == "connect") config.Connect = true; 
      if (server.argName(i) == "interval") config.Interval =  server.arg(i).toInt(); 
      if (server.argName(i) == "wakecount") config.WakeCount =  server.arg(i).toInt(); 
      if (server.argName(i) == "server") config.streamServerURL = urldecode(server.arg(i)); 
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
	server.send ( 200, "text/html", PAGE_AdminGeneralSettings ); 
	debugln("",__FUNCTION__); 
	}

void send_general_configuration_values_html() {
	String values ="";
	values += "devicename|" + (String) config.DeviceName +  "|input\n";
  values += "interval|"   + (String) config.Interval +  "|input\n";
  values += "wakecount|"  + (String) config.WakeCount +  "|input\n";
	values += "logging|"    + (String) (config.Logging ? "checked" : "") + "|chk\n";
  values += "connect|"    + (String) (config.Connect ? "checked" : "") + "|chk\n";
  values += "server|"     + (String) config.streamServerURL + "|input\n";
  values += "blinked|"    + (String) (digitalRead(WAS_BLINK)? "YES": "NO") + "|div\n";
  digitalWrite(CLEAR_BLINK, LOW);
	server.send ( 200, "text/plain", values);
	debugln("",__FUNCTION__); 
  digitalWrite(CLEAR_BLINK, HIGH);
  }
  
