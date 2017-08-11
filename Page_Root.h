
/* 
 *  TODO: You can't use libraries from the internet here if the client is directly connected to the ESP... 
 *  because then your client doesn't have access to the internet, now does it? Switch back to microAjax.
 */

const char PAGE_Root[] PROGMEM = R"=====(
<!doctype html>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<script src="microajax.js"></script> 
<html>
  <head>
    <title>ESP8266 IOT Serial to Web</title>
    <style>
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }
    </style>
  <meta charset="utf-8" />
  </head>
  <body>
    <pre><div id='log'></div></pre>
    <p>
    <form id='msg' action='data' method='get'>
    <input id='txt' name='text' type='text'></input></form></p>
    <p><a href="/admin.html">Settings</a> <a href="/file?start=1">Stream</a></p>
    <script type='text/javascript' src='http://ajax.googleapis.com/ajax/libs/jquery/1.3/jquery.min.js'></script>
    <script type='text/javascript'>
$('#msg').submit(function(){ 
    var clientmsg = $('#txt').val()+'\n';
    var log=$('#log');
    log.html(log.html()+'<b>'+clientmsg+'</b>');
    $.get('data', {text: clientmsg}, function(html){
        log.html(log.html()+html);
        });
    $('#txt').attr('value', '');
    return false;
  });
function loadLog(){
  var log=$('#log');
  $.ajax({
      url: 'data',
      cache: false,
      success: function(html){
        log.html(log.html()+html);
        },
    });
  };
 setInterval (loadLog, 2500);
      </script>
  </body>
</html>

)=====";

void handle_root() {
  server.send(200, "text/html", PAGE_Root);
  //delay(100); //why is this here? TODO: try without.
  debug(".","");
}
//Change the setInterval number above for faster or slower updates from serial to web

/*
void sendRootPage() {        
    debug("root");
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
       for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
            
        }
    }
    server.send ( 200, "text/html", PAGE_Root ); 
}

*/
