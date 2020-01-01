#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <stdio.h>
#include <stdlib.h>


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

WebServer server(80);

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

  server.on("/", handleRoot);
  server.on("/wheels", handleWheels);
  // server.on("/inline", []() {
  //   server.send(200, "text/plain", "this works as well");
  // });
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("Server started");
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", myIP);
}
void handleRoot() {
  String message = "<!doctype html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no\"><meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\"><title>Car</title><style>.main{display:flex;justify-content:space-between;--size:300px}steering-wheel{display:block;overflow:hidden}steering-wheel,svg{width:var(--size);height:var(--size)}path{fill:#f525e8;fill:pink;transform-origin:50% 50%}.speed-control-wrapper{position:relative}input[type=range]{--width:80px;-webkit-appearance:slider-vertical;width:var(--width);padding:0 5px;height:var(--size)}::-webkit-slider-runnable-track{background:#ddd}::-webkit-slider-thumb{width:var(--width);min-width:var(--width);--height:60px;height:var(--height);min-height:var(--height);background:#fff;border:2px solid #999}.debug-monitor{height:200px;bottom:0;position:absolute;left:0;right:0;overflow:auto;border:1px solid #000;padding:5px}.danger{color:red}</style></head><body><div class=\"main\"><div class=\"wheel\"><steering-wheel id=\"servo\" value=\"0\" min-angle=\"-50\" max-angle=\"50\" step=\"2\"><svg viewBox=\"0 0 32 32\"><path d=\"M16,0C7.164,0,0,7.164,0,16s7.164,16,16,16s16-7.164,16-16S24.836,0,16,0z M16,4 c5.207,0,9.605,3.354,11.266,8H4.734C6.395,7.354,10.793,4,16,4z M16,18c-1.105,0-2-0.895-2-2s0.895-2,2-2s2,0.895,2,2 S17.105,18,16,18z M4,16c5.465,0,9.891,5.266,9.984,11.797C8.328,26.828,4,21.926,4,16z M18.016,27.797 C18.109,21.266,22.535,16,28,16C28,21.926,23.672,26.828,18.016,27.797z\"/></svg></steering-wheel><span id=\"wheel-angle\"></span></div><div class=\"speed-control-wrapper\"><input type=\"range\" min=\"-40\" max=\"40\" value=\"0\" step=\"10\" id=\"speed-control\" orient=\"vertical\"></div></div><span id=\"speed-control-value\"></span><div class=\"debug-monitor\"></div><script>!function(e){var t={};function n(i){if(t[i])return t[i].exports;var s=t[i]={i:i,l:!1,exports:{}};return e[i].call(s.exports,s,s.exports,n),s.l=!0,s.exports}n.m=e,n.c=t,n.d=function(e,t,i){n.o(e,t)||Object.defineProperty(e,t,{enumerable:!0,get:i})},n.r=function(e){\"undefined\"!=typeof Symbol&&Symbol.toStringTag&&Object.defineProperty(e,Symbol.toStringTag,{value:\"Module\"}),Object.defineProperty(e,\"__esModule\",{value:!0})},n.t=function(e,t){if(1&t&&(e=n(e)),8&t)return e;if(4&t&&\"object\"==typeof e&&e&&e.__esModule)return e;var i=Object.create(null);if(n.r(i),Object.defineProperty(i,\"default\",{enumerable:!0,value:e}),2&t&&\"string\"!=typeof e)for(var s in e)n.d(i,s,function(t){return e[t]}.bind(null,s));return i},n.n=function(e){var t=e&&e.__esModule?function(){return e.default}:function(){return e};return n.d(t,\"a\",t),t},n.o=function(e,t){return Object.prototype.hasOwnProperty.call(e,t)},n.p=\"\",n(n.s=0)}([function(e,t,n){n(1),e.exports=n(3)},function(e,t,n){\"use strict\";Object.defineProperty(t,\"__esModule\",{value:!0});const i=n(2),s=document.querySelector(\"#servo\"),o=document.getElementById(\"wheel-angle\"),r=document.querySelector(\".debug-monitor\"),u=100;let l,h=!1;var c;function a(e){const t=Date.now();h=!0,console.log((new Date).toISOString());const n=new URL(\"/wheels\",location.origin);n.search=new URLSearchParams(e).toString(),fetch(n.href,{cache:\"no-cache\",method:\"PUT\"}).then(e=>e.text()).then(n=>{const i=Date.now()-t;!function(e,t=\"\"){const n=document.createElement(\"div\");n.innerText=e,n.className=t,r.appendChild(n),r.scrollTop=1e8}(`${JSON.stringify(e)}, ${n}, ${i}ms`,i>u?\"danger\":\"\")}).finally(()=>{h=!1,l&&(a(l),l=null)})}function d(e){h?l=Object.assign({},l,e):a(e)}!function(e){e.angle=\"driveWheelAngle\",e.power=\"drivePower\"}(c||(c={})),s.addEventListener(i.DrivingWheelTurnEventName,(function(){const e=s.value.toString();d({[c.angle]:e}),o.innerText=e}));const m=document.getElementById(\"speed-control\"),g=document.getElementById(\"speed-control-value\");m.addEventListener(\"input\",e=>{e.stopPropagation();const t=+m.value,n=t?Math.sign(t)*(Math.abs(t)+60):0;d({[c.power]:n.toString()}),g.innerText=m.value})},function(e,t,n){\"use strict\";n.r(t),n.d(t,\"DrivingWheelTurnEventName\",(function(){return s})),n.d(t,\"SteeringWheel\",(function(){return o}));const i=100/60,s=\"input\";class o extends HTMLElement{constructor(){super(),this.interacting=!1,this.visibleAngle=0,this.curAngle=0,this.curStep=1,this.userActionInitialAngle=0,this.minPossibleAngle=-1/0,this.maxPossibleAngle=1/0,this.onMouseDown=this.onMouseDown.bind(this),this.onMouseMove=this.onMouseMove.bind(this),this.onMouseUp=this.onMouseUp.bind(this),this.onTouchStart=this.onTouchStart.bind(this);const e=this.attachShadow({mode:\"open\"});this.wheel=document.createElement(\"div\"),this.wheel.appendChild(document.createElement(\"slot\")),e.appendChild(this.wheel),this.prepareStyles()}get value(){return this.curAngle}set value(e){this.setCurAngle(e)}get minAngle(){return this.minPossibleAngle}set minAngle(e){this.minPossibleAngle=e}get maxAngle(){return this.maxPossibleAngle}set maxAngle(e){this.maxPossibleAngle=e}get step(){return this.curStep}set step(e){this.curStep=e}connectedCallback(){for(const e of[\"minAngle\",\"maxAngle\",\"value\",\"step\"])this.upgradeProperty(e);this.initEvents()}disconnectedCallback(){this.removeEvents(),this.onMouseUp()}attributeChangedCallback(e,t,n){switch(e){case\"value\":return void(Number.isFinite(Number(n))&&this.setCurAngle(Number(n)));case\"min-angle\":return void(this.minAngle=Number.isFinite(Number(n))?Number(n):this.minAngle);case\"max-angle\":return void(this.maxAngle=Number.isFinite(Number(n))?Number(n):this.maxAngle);case\"step\":const e=Number(n);return void(this.step=Number.isFinite(e)&&e>0?Number(n):this.step)}}prepareStyles(){const e=document.createElement(\"style\");e.textContent=\" div { position: absolute;  }\\n                          slot { display: block; font-size: 0;} \",this.shadowRoot.appendChild(e)}upgradeProperty(e){if(this.hasOwnProperty(e)){const t=this[e];delete this[e],this[e]=t}}initEvents(){this.wheel.addEventListener(\"mousedown\",this.onMouseDown),this.wheel.addEventListener(\"touchstart\",this.onTouchStart)}removeEvents(){this.wheel.removeEventListener(\"mousedown\",this.onMouseDown),this.wheel.removeEventListener(\"touchstart\",this.onTouchStart)}moveSlowlyTo(e,t=!1){const n=Math.abs(this.curAngle-e)*i;this.wheel.style.transition=`transform ${n/1e3}s ease-out 0s`,this.setCurAngle(e,t),setTimeout(()=>this.wheel.style.transition=null,n+50)}onMouseDown(e){this.interacting||(document.addEventListener(\"mousemove\",this.onMouseMove),document.addEventListener(\"mouseup\",this.onMouseUp)),this.interacting=!0,this.userActionInitialAngle=this.getAngle(e)}onTouchStart(e){if(!this.interacting){const t=e.changedTouches[0];this.touchIdentifier=t.identifier,document.addEventListener(\"touchmove\",this.onMouseMove),document.addEventListener(\"touchend\",this.onMouseUp),this.userActionInitialAngle=this.getAngle(t),e.preventDefault()}this.interacting=!0}onMouseMove(e){let t=null;if(e instanceof TouchEvent){if(t=Array.prototype.find.call(e.changedTouches,e=>e.identifier===this.touchIdentifier),!t)return}else t=e;const n=this.getAngle(t),i=n-this.userActionInitialAngle;this.userActionInitialAngle=n;const s=[i-360,i,i+360],o=s.reduce((e,t)=>Math.abs(e)<Math.abs(t)?e:t,s.pop());this.setCurAngle(this.visibleAngle+o,!0)}setCurAngle(e,t=!1){let n;n=e<this.minAngle?this.minAngle:e>this.maxAngle?this.maxAngle:e;const i=Math.round(n/this.step)*this.step;this.visibleAngle=t?n:i,this.wheel.style.transform=`rotate(${this.visibleAngle}deg)`,t&&i!==this.curAngle&&(this.curAngle=i,this.dispatchEvent(new Event(s)))}getAngle(e){const t=this.wheel.getBoundingClientRect(),n=(t.left+t.right)/2,i=(t.top+t.bottom)/2-e.clientY,s=e.clientX-n,o=90-Math.atan2(i,s)*(180/Math.PI);return o>180?o-360:o}onMouseUp(){this.removeDynamicEventListeners(),this.moveSlowlyTo(this.curAngle),this.interacting=!1}removeDynamicEventListeners(){document.removeEventListener(\"mousemove\",this.onMouseMove),document.removeEventListener(\"mouseup\",this.onMouseUp),document.removeEventListener(\"touchmove\",this.onMouseMove),document.removeEventListener(\"touchend\",this.onMouseUp)}}o.observedAttributes=[\"value\",\"min-angle\",\"max-angle\",\"step\"],customElements.define(\"steering-wheel\",o)},function(e,t,n){}]);</script></body></html>\n\n";
  server.send(200, "text/html", message);
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
void showMessages() {
  String message = "";
  for (uint8_t i = 0; i < server.args(); i++) {
    String arg = server.argName(i);
    message += "argName ";
    message += arg;
    message += "; argValue ";
    message += server.arg(i);
    Serial.println(message);
  }

}
void handleWheels() {
  unsigned long start = micros();
  showMessages();
  unsigned long afterMessages = micros();
  if (server.hasArg("drivePower")) {
    setPower(getValue(server.arg("drivePower")));
  }
  unsigned long afterPower = micros();
  if (server.hasArg("driveWheelAngle")) {
    setAngle(getValue(server.arg("driveWheelAngle")));
  }
  unsigned long afterAngle = micros();
  char response[100];
  snprintf(response, 100, "message: %d, power: %d, angle: %d", afterMessages - start, afterPower - afterMessages, afterAngle - afterPower);
  server.send(200, "text/plain", response);
  server.setContentLength(50);

  unsigned long afterSend = micros();
  char debugMessage[100];
  snprintf(debugMessage, 100, "message: %d, power: %d, angle: %d, after send: %d", afterMessages - start, afterPower - afterMessages, afterAngle - afterPower, afterSend - afterAngle);
  Serial.println(debugMessage);

}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void loop(void) {
  server.handleClient();
  dnsServer.processNextRequest();
}