#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <stdio.h>
#include <stdlib.h>
#include "ESPAsyncWebServer.h"
#include "./page.c"

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
  request->send(200, "text/html", page);
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
  const float minEfTime = 500 + (maxTime - minTime) * 0.04;
  const float maxEfTime = 2260 - (maxTime - minTime) * 0.14;
  const float microSecondsCount = 1000000;
  const float maxPwm = 65536;
  const float pwm = maxPwm * (minEfTime + (maxEfTime - minEfTime) * (-value - minAngle) / (maxAngle - minAngle)) / microSecondsCount  * servoFreq;
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
  char response[60];
  snprintf(response, 61, "power: %d, angle: %d", drivePower, angle);
  Serial.println(response);
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
