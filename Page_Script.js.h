
const char PAGE_microajax_js[] PROGMEM = R"=====(
function microAjax(B,A){
  this.bindFunction=function(E,D){
    return function(){
      return E.apply(D,[D])
      }
    };
  this.stateChange=function(D){
    if(this.request.readyState==4 && this.request.status==200){
      this.callbackFunction(this.request.responseText)
      }
    };
  this.getRequest=function(){
    if(window.XMLHttpRequest) return new XMLHttpRequest();
    else{ if(window.ActiveXObject) return new ActiveXObject("Microsoft.XMLHTTP"); }
    return false
    };
  this.postBody=(arguments[2]||"");
  this.callbackFunction=A;
  this.url=B;
  this.request=this.getRequest();
  if(this.request){
    var C=this.request;
    C.onreadystatechange=this.bindFunction(this.stateChange,this);
    if(this.postBody!==""){
      C.open("POST",B,true);
      C.setRequestHeader("X-Requested-With","XMLHttpRequest");
      C.setRequestHeader("Content-type","application/x-www-form-urlencoded");
      C.setRequestHeader("Connection","close")
      }
    else{
      C.open("GET",B,true)
      }
    C.send(this.postBody)
    }
  };

function setValues(url) {
	microAjax(url, function (res) {
    res.split(String.fromCharCode(10)).forEach( function(entry) {
  		fields = entry.split("|");
  		if(fields[2] == "input") {
        document.getElementById(fields[0]).value = fields[1];
        }
      else if(fields[2] == "div")	{
        document.getElementById(fields[0]).innerHTML  = fields[1];
        }
      else if(fields[2] == "chk") {
        document.getElementById(fields[0]).checked  = fields[1];
        }
      });
    if (typeof setValuesDone === 'function') setValuesDone();
    });
  }

<<<<<<< HEAD
)=====";
=======
)=====";
>>>>>>> 83fa7e5c92ed360d55aadc0e61294994e66eff35
