#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <stdio.h>
#include <stdlib.h>
#include "ESPAsyncWebServer.h"

#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED
const int ledPin = 2;
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

const int servoPin = 4;
const int servoChannel = 1;
const int servoResolution = 16;
const int servoFreq = 50;

const int engEnabledPin = 5;
const int engFirstPin = 16;
const int engSecondPin = 17;
const int engChanel = 2;
const int engResolution = 8;
const int engFreq = 20000;
int angle = 0;
int drivePower = 0;

// Set these to your desired credentials.
const char *ssid = "my-car";
const char *password = "my-super-car";
// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

AsyncWebServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(engEnabledPin, OUTPUT);
  pinMode(engFirstPin, OUTPUT);
  pinMode(engSecondPin, OUTPUT);

  // ledcSetup(ledChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  // ledcAttachPin(ledPin, ledChannel);

  ledcSetup(servoChannel, servoFreq, servoResolution);
  ledcAttachPin(servoPin, servoChannel);

  ledcSetup(engChanel, engFreq, engResolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(engEnabledPin, engChanel);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/wheels", HTTP_PUT, handleWheels);
  // server.on("/inline", []() {
  //   server.send(200, "text/plain", "this works as well");
  // });
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("Server started");
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", myIP);
}
void handleRoot(AsyncWebServerRequest *request) {
  String message = "<!doctype html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no\"><meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\"><title>Car</title><style>.main{display:flex;justify-content:space-between;--size:300px}steering-wheel{display:block;overflow:hidden}steering-wheel,svg{width:var(--size);height:var(--size)}path{fill:#f525e8;fill:pink;transform-origin:50% 50%}.speed-control-wrapper{width:calc(var(--size)/2);overflow:hidden;position:relative}.power-arrow-placeholder{height:var(--size);width:var(--size);border-radius:50%;border:1px solid #000;position:relative}.power-arrow{--arrow-width:80px;position:absolute;width:0;height:0;border-top:calc(var(--arrow-width)/2) solid transparent;border-bottom:calc(var(--arrow-width)/2) solid transparent;border-right:var(--arrow-width) solid red;left:20px;top:50%;margin-top:calc(var(--arrow-width)/-2);z-index:-1}.debug-monitor{height:200px;bottom:0;position:absolute;left:0;right:0;overflow:auto;border:1px solid #000;padding:5px}.danger{color:red}</style></head><body><label class=\"monitor-switcher-placeholder\"><input type=\"checkbox\"> Show debug monitor</label><div class=\"main\"><div class=\"wheel\"><steering-wheel id=\"servo\" value=\"0\" min-angle=\"-50\" max-angle=\"50\" step=\"2\" end-action-value=\"0\"><svg viewBox=\"0 0 32 32\"><path d=\"M16,0C7.164,0,0,7.164,0,16s7.164,16,16,16s16-7.164,16-16S24.836,0,16,0z M16,4 c5.207,0,9.605,3.354,11.266,8H4.734C6.395,7.354,10.793,4,16,4z M16,18c-1.105,0-2-0.895-2-2s0.895-2,2-2s2,0.895,2,2 S17.105,18,16,18z M4,16c5.465,0,9.891,5.266,9.984,11.797C8.328,26.828,4,21.926,4,16z M18.016,27.797 C18.109,21.266,22.535,16,28,16C28,21.926,23.672,26.828,18.016,27.797z\"/></svg></steering-wheel><span id=\"wheel-angle\"></span></div><div class=\"speed-control-wrapper\"><steering-wheel id=\"power\" value=\"0\" step=\"2\" direction=\"v\" end-action-value=\"0\" min-angle=\"-90\" max-angle=\"90\"><div class=\"power-arrow-placeholder\"><div class=\"power-arrow\"></div></div></steering-wheel></div></div><span id=\"speed-control-value\"></span><div class=\"debug-monitor\" hidden></div><script>!function(e){var t={};function n(i){if(t[i])return t[i].exports;var o=t[i]={i:i,l:!1,exports:{}};return e[i].call(o.exports,o,o.exports,n),o.l=!0,o.exports}n.m=e,n.c=t,n.d=function(e,t,i){n.o(e,t)||Object.defineProperty(e,t,{enumerable:!0,get:i})},n.r=function(e){\"undefined\"!=typeof Symbol&&Symbol.toStringTag&&Object.defineProperty(e,Symbol.toStringTag,{value:\"Module\"}),Object.defineProperty(e,\"__esModule\",{value:!0})},n.t=function(e,t){if(1&t&&(e=n(e)),8&t)return e;if(4&t&&\"object\"==typeof e&&e&&e.__esModule)return e;var i=Object.create(null);if(n.r(i),Object.defineProperty(i,\"default\",{enumerable:!0,value:e}),2&t&&\"string\"!=typeof e)for(var o in e)n.d(i,o,function(t){return e[t]}.bind(null,o));return i},n.n=function(e){var t=e&&e.__esModule?function(){return e.default}:function(){return e};return n.d(t,\"a\",t),t},n.o=function(e,t){return Object.prototype.hasOwnProperty.call(e,t)},n.p=\"\",n(n.s=0)}([function(e,t,n){n(1),e.exports=n(3)},function(e,t,n){\"use strict\";Object.defineProperty(t,\"__esModule\",{value:!0});const i=n(2),o=document.querySelector(\"#servo\"),s=document.getElementById(\"wheel-angle\"),r=document.querySelector(\".debug-monitor\"),u=100;let l,c=!1;var h;function a(e){const t=Date.now();c=!0;const n=new URL(\"/wheels\",location.origin);n.search=new URLSearchParams(e).toString();const i=new XMLHttpRequest;i.timeout=100,i.addEventListener(\"load\",()=>{const n=Date.now()-t;!function(e,t=\"\"){const n=document.createElement(\"div\");n.innerText=e,n.className=t,r.appendChild(n),r.scrollTop=1e8}(`${JSON.stringify(e)}, ${i.responseText}, ${n}ms`,n>u?\"danger\":\"\")}),i.addEventListener(\"loadend\",()=>{c=!1,l&&(a(l),l=null)}),i.open(\"PUT\",n.href),i.send()}function d(e){c?l=Object.assign({},l,e):a(e)}!function(e){e.angle=\"driveWheelAngle\",e.power=\"drivePower\"}(h||(h={})),function(){const e=document.querySelector(\".monitor-switcher-placeholder input\");e.addEventListener(\"change\",()=>{r.hidden=!e.checked})}(),o.addEventListener(i.DrivingWheelTurnEventName,(function(){const e=o.value.toString();d({[h.angle]:e}),s.innerText=e}));const m=document.getElementById(\"power\"),v=document.getElementById(\"speed-control-value\");let g=0;m.addEventListener(i.DrivingWheelTurnEventName,e=>{const t=Math.abs(m.value)<10?0:m.value;if(t===g)return;g=t;const n=g?Math.sign(g)*(Math.abs(g/2)+55):0;d({[h.power]:n.toString()}),v.innerText=m.value.toString()})},function(e,t,n){\"use strict\";n.r(t),n.d(t,\"DrivingWheelTurnEventName\",(function(){return o})),n.d(t,\"SteeringWheel\",(function(){return l}));const i=100/60,o=\"input\",s=90,r=-90;var u;!function(e){e.Vertical=\"v\",e.Horizontal=\"h\"}(u||(u={}));class l extends HTMLElement{constructor(){super(),this.interacting=!1,this.visibleAngle=0,this.curAngle=0,this.curStep=1,this.curDirection=u.Horizontal,this.curEndActionValue=NaN,this.minPossibleAngle=r,this.maxPossibleAngle=s,this.onMouseDown=this.onMouseDown.bind(this),this.onMouseMove=this.onMouseMove.bind(this),this.onMouseUp=this.onMouseUp.bind(this),this.onTouchStart=this.onTouchStart.bind(this);const e=this.attachShadow({mode:\"open\"});this.wheel=document.createElement(\"div\"),this.wheel.appendChild(document.createElement(\"slot\")),e.appendChild(this.wheel),this.prepareStyles()}get value(){return this.curAngle}set value(e){this.setCurAngle(e)}get minAngle(){return this.minPossibleAngle}set minAngle(e){this.minPossibleAngle=Number.isFinite(e)?e:r}get maxAngle(){return this.maxPossibleAngle}set maxAngle(e){this.maxPossibleAngle=Number.isFinite(e)?e:s}get step(){return this.curStep}set step(e){this.curStep=e}get direction(){return this.curDirection}set direction(e){this.curDirection=e===u.Vertical?u.Vertical:u.Horizontal}get endActionValue(){return this.curEndActionValue}set endActionValue(e){this.curEndActionValue=e}connectedCallback(){for(const e of[\"minAngle\",\"maxAngle\",\"value\",\"step\",\"direction\",\"endActionValue\"])this.upgradeProperty(e);this.initEvents()}disconnectedCallback(){this.removeEvents(),this.onMouseUp()}attributeChangedCallback(e,t,n){switch(e){case\"value\":return void(Number.isFinite(Number(n))&&this.setCurAngle(Number(n)));case\"min-angle\":return void(this.minAngle=Number.isFinite(Number(n))?Number(n):r);case\"max-angle\":return void(this.maxAngle=Number.isFinite(Number(n))?Number(n):s);case\"step\":const e=Number(n);return void(this.step=Number.isFinite(e)&&e>0?Number(n):this.step);case\"direction\":return void(this.direction=n);case\"end-action-value\":return void(this.endActionValue=Number(n))}}prepareStyles(){const e=document.createElement(\"style\");e.textContent=\" div { position: absolute;  }\\n                          slot { display: block; font-size: 0;} \",this.shadowRoot.appendChild(e)}upgradeProperty(e){if(this.hasOwnProperty(e)){const t=this[e];delete this[e],this[e]=t}}initEvents(){this.wheel.addEventListener(\"mousedown\",this.onMouseDown),this.wheel.addEventListener(\"touchstart\",this.onTouchStart)}removeEvents(){this.wheel.removeEventListener(\"mousedown\",this.onMouseDown),this.wheel.removeEventListener(\"touchstart\",this.onTouchStart)}moveSlowlyTo(e,t=!1){const n=Math.abs(this.curAngle-e)*i;this.wheel.style.transition=`transform ${n/1e3}s ease-out 0s`,this.setCurAngle(e,t),setTimeout(()=>this.wheel.style.transition=null,n+50)}onMouseDown(e){this.interacting||(document.addEventListener(\"mousemove\",this.onMouseMove),document.addEventListener(\"mouseup\",this.onMouseUp)),this.interacting=!0}onTouchStart(e){if(!this.interacting){const t=e.changedTouches[0];this.touchIdentifier=t.identifier,document.addEventListener(\"touchmove\",this.onMouseMove),document.addEventListener(\"touchend\",this.onMouseUp),e.preventDefault()}this.interacting=!0}onMouseMove(e){let t=null;if(e instanceof TouchEvent){if(t=Array.prototype.find.call(e.changedTouches,e=>e.identifier===this.touchIdentifier),!t)return}else t=e;const n=this.getAngle(t);this.setCurAngle(n,!0)}setCurAngle(e,t=!1){const n=Math.max(Math.min(this.maxAngle,e),this.minAngle),i=Math.round(n/this.step)*this.step;this.visibleAngle=t?n:i,this.wheel.style.transform=`rotate(${this.visibleAngle}deg)`,t&&i!==this.curAngle&&(this.curAngle=i,this.dispatchEvent(new Event(o)))}getAngle(e){const t=this.getBoundingClientRect();return this.direction===u.Horizontal?(e.clientX-(t.left+.1*t.width))/(.8*t.width)*(this.maxAngle-this.minAngle)+this.minAngle:(e.clientY-(t.top+.1*t.height))/(.8*t.height)*(this.minAngle-this.maxAngle)-this.minAngle}onMouseUp(e=null){if(e&&e instanceof TouchEvent){if(!Array.prototype.find.call(e.changedTouches,e=>e.identifier===this.touchIdentifier))return}this.removeDynamicEventListeners(),Number.isFinite(this.endActionValue)?this.moveSlowlyTo(this.endActionValue,!0):this.moveSlowlyTo(this.curAngle),this.interacting=!1}removeDynamicEventListeners(){document.removeEventListener(\"mousemove\",this.onMouseMove),document.removeEventListener(\"mouseup\",this.onMouseUp),document.removeEventListener(\"touchmove\",this.onMouseMove),document.removeEventListener(\"touchend\",this.onMouseUp)}}l.observedAttributes=[\"value\",\"min-angle\",\"max-angle\",\"step\",\"direction\",\"end-action-value\"],customElements.define(\"steering-wheel\",l)},function(e,t,n){}]);</script></body></html>";
  request->send(200, "text/html", message);
}

int getValue(String unParsedValue) {
  char charBuf[unParsedValue.length() + 1];
  unParsedValue.toCharArray(charBuf, unParsedValue.length()+1);
  return atoi(charBuf);
}

void setPower(int value) {
  if (value == drivePower) {
    return;
  }
  drivePower = value;
  const float curPower = abs(value) * 2.55 ;
  char debugMessage[30];
  snprintf(debugMessage, 30, "curPower: %d", (int)curPower);
  Serial.println(debugMessage);
  if (value < 0) {
    digitalWrite(engSecondPin, LOW);
    digitalWrite(engFirstPin, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  if (value > 0) {
    digitalWrite(engFirstPin, LOW);
    digitalWrite(engSecondPin, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
  }
  ledcWrite(engChanel, (int)curPower);
}

void setAngle(int value) {
  if (value == angle) {
    return;
  }
  angle = value;
  const float minAngle = -50;
  const float maxAngle = 50;
  const float minTime = 500; //ms
  const float maxTime = 2260; // ms
  const float microSecondsCount = 1000000;
  const float maxPwm = 65536;
  const float pwm = maxPwm * (minTime + (maxTime - minTime) * (-value - minAngle) / (maxAngle - minAngle)) / microSecondsCount  * servoFreq;
  char debugMessage[50];
  snprintf(debugMessage, 30, "PWM: %d", (int)pwm);
  Serial.println(debugMessage);
  ledcWrite(servoChannel, (int)pwm);
}
void showMessages(AsyncWebServerRequest *request) {
  int params = request->params();
  String message = "";
  for (int i=0;i < params;i++) {
    AsyncWebParameter* p = request->getParam(i);
    message += "argName ";
    message += p->name().c_str();
    message += "; argValue ";
    message += p->value().c_str();
    Serial.println(message);
  }

}
void handleWheels(AsyncWebServerRequest *request) {
  unsigned long start = micros();
  showMessages(request);
  unsigned long afterMessages = micros();

  if (request->hasParam("drivePower")) {
    setPower(getValue(request->getParam("drivePower")->value()));
  }
  unsigned long afterPower = micros();
  if (request->hasParam("driveWheelAngle")) {
    setAngle(getValue( request->getParam("driveWheelAngle")->value()));
  }
  unsigned long afterAngle = micros();
  char response[100];
  snprintf(response, 100, "message: %d, power: %d, angle: %d", afterMessages - start, afterPower - afterMessages, afterAngle - afterPower);
  request->send(200, "text/plain", "ok");

  unsigned long afterSend = micros();
  char debugMessage[100];
  snprintf(debugMessage, 100, "message: %d, power: %d, angle: %d, after send: %d", afterMessages - start, afterPower - afterMessages, afterAngle - afterPower, afterSend - afterAngle);
  Serial.println(debugMessage);

}

void handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void loop(void) {
  dnsServer.processNextRequest();
}
