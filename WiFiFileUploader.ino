#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include "settings.h"

const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);
File fsUploadFile;

const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html><html lang="en"><head><meta name="viewport"content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no"><title>SD Editor</title><style type="text/css"media="screen">body{background-color:#f2f2f2;}
ul{list-style-type:decimal}
li{margin:5px;cursor:pointer}
h1{font-size:2rem;text-align:center;margin:0 0 2em;}
input{border:none;padding:0.55em;width:9em;border-radius:2px;}
button{letter-spacing:1px;font-size:12px;display:inline-block;position:relative;border-radius:2px;overflow:hidden;-webkit-transition:0.2s ease-out;transition:0.2s ease-out;font-family:sans-serif;cursor:pointer;border:1px solid white;padding:0.6em 1.2em;margin:5px;background-color:transparent;text-align:center;color:white;}
button:focus{outline:none;}
button:hover{box-shadow:0 3px 6px 0 rgba(0,0,0,0.22),0 4px 4px 0 rgba(0,0,0,0.35);color:#222;text-decoration:none;background-color:rgba(255,255,255,0.6);}
button:active{box-shadow:0 1px 3px 0 rgba(0,0,0,0.44),0 1px 1px 0 rgba(0,0,0,0.68);}
button:focus::before{display:block;height:150px;width:150px;top:-58px;left:-30px;opacity:0;content:"";background:#AAA;border-radius:1000px;position:absolute;animation-name:fade;animation-timing-function:ease-out;animation-duration:1s;z-index:-2;}@keyframes fade{0%{opacity:1;transform:scale(0);}
50%{opacity:1;transform:scale(1);}
100%{opacity:0;}}
li{font-size:1.5rem;text-align:left;height:40px;padding-top:16px;padding-left:16px;display:block;border-bottom:1px solid rgba(0,0,0,.12);}.container{position:relative;max-width:40rem;margin:5rem auto;background:#fff;width:100%;}.container::before{content:'';position:absolute;top:0;left:0;right:0;bottom:0;box-shadow:0 8px 10px 1px rgba(0,0,0,0.14),0 3px 14px 2px rgba(0,0,0,0.12),0 5px 5px-3px rgba(0,0,0,0.2);}.close{position:absolute;right:1em;width:1em;height:1em;opacity:0.7;}.close:hover{opacity:1;}.close:before,.close:after{position:absolute;left:15px;content:' ';height:33px;width:2px;background-color:#333;}.close:before{transform:rotate(45deg);}.close:after{transform:rotate(-45deg);}#uploader{padding-left:10px;background-color:#00A;color:#FFF;background-image:linear-gradient(to right,#0060b3 50%,#999 100%)}#i{position:fixed;bottom:1em;left:-100%;width:90%;margin-left:2%;background:#33691e;font-size:2em;color:white;padding:0.5em;text-align:center;border-radius:5px;-webkit-transition:0.2s ease-out;transition:0.2s ease-out;box-shadow:0 1px 6px 0 rgba(0,0,0,0.44),0 3px 8px 0 rgba(0,0,0,0.68);}</style><script>var treeRoot;function createFileUploader(b){var d;var f=document.getElementById('u_h');var c=document.createElement('button');c.innerHTML='Upload';document.getElementById(b).appendChild(c);function h(){if(d.readyState==4){if(d.status!=200){alert('ERROR['+d.status+']: '+d.responseText)}else{setTimeout(function(){ok();httpGet(treeRoot,'/');},500);}}}
c.onclick=function(k){if(f.files.length===0){return}
d=new XMLHttpRequest();d.onreadystatechange=h;var j=new FormData();j.append('data',f.files[0],f.value.replace(/^.*[\\\/]/,''));d.open('POST','/e');d.send(j);};}
function del(n){if(confirm("Delite "+n+"?")){xmlHttp=new XMLHttpRequest();xmlHttp.onreadystatechange=setTimeout(function(){ok();httpGet(treeRoot,'/');},500);var formData=new FormData();formData.append("path",'/'+n);xmlHttp.open("DELETE","/e");xmlHttp.send(formData);}}
function addList(d,g,a){var c='<ul>';for(var b=1;b<a.length;b++){if(a[b].length>0);c+='<li>'+a[b]+'<i class=\u0022close\u0022 onClick=\u0022del(\u0027'+a[b]+'\u0027)\u0022></i></li>';}
c+='</ul>';d.innerHTML=c}
function getCb(a,b){return function(){if(xmlHttp.readyState==4){console.log(xmlHttp.responseText);if(xmlHttp.status==200){addList(a,b,xmlHttp.responseText.split('/'));}}}}
function httpGet(a,b){if(b!='/'){a.onclick=function(){}}
xmlHttp=new XMLHttpRequest(a,b);xmlHttp.onreadystatechange=getCb(a,b);xmlHttp.open('GET','/l',true);xmlHttp.send(null)}
function onBodyLoad(){createFileUploader('uploader');treeRoot=document.getElementById('tree');httpGet(treeRoot,'/');};function ok(){var i=document.getElementById('i');i.style.left="0";setTimeout(function(){document.getElementById('i').style.left="-100%";},3000);}</script></head><body onload='onBodyLoad()'><div class='container'><div id='uploader'><input type="file"id="u_h"
style="position: absolute; display: block; overflow: hidden; width: 0; height: 0; border: 0; padding: 0;"
onchange="document.getElementById('u_v').value = this.value.replace(/^.*[\\\/]/, '');"/><input type="text"readonly="1"id="u_v"
onclick="document.getElementById('u_h').click();"/><button onclick="document.getElementById('u_h').click();">Browse</button></div><div id='tree'style='top:1em'></div></div><div id="i">The operation was successful</div></body></html>
)=====";


void handleFileList() {
  Dir dir = SPIFFS.openDir("/");

  String output = "";
  while (dir.next()) {
    File entry = dir.openFile("r");
    output += String(entry.name());
    entry.close();
  }
  
  server.send(200, "text/json", output);
  output = String();
  Serial.print(F("Free "));
  Serial.println(ESP.getFreeHeap());
}

void handleFileUpload() {
  if (server.uri() != "/e") {
    return;
  }
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    Serial.print(F("handleFileUpload Name: ")); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {
      fsUploadFile.close();
    }
    Serial.print(F("handleFileUpload Size: ")); Serial.println(upload.totalSize);
  }
}

void handleFileDelete() {
  const char *txtplain = "text/plain";
  if (server.args() == 0) {
    return server.send(500, txtplain, "BAD ARGS");
  }
  String path = server.arg(0);
  Serial.print(F("handleFileDelete: ")); Serial.println(path);
  if (path == "/") {
    return server.send(500, txtplain, "BAD PATH");
  }
  if (!SPIFFS.exists(path)) {
    return server.send(404, txtplain, "404");
  }
  SPIFFS.remove(path);
  server.send(200, txtplain, "");
  path = String();
}

void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

void serverSetup() {
  Serial.println();
  Serial.print(F("Configuring access point..."));
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print(F("AP IP address: "));
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/l", HTTP_GET, handleFileList);
  server.on("/e", HTTP_DELETE, handleFileDelete);
  server.on("/e", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);
  server.begin();
  delay(50);
  Serial.println(F("HTTP server started"));
}

void serverLoop() {
  server.handleClient();
}
