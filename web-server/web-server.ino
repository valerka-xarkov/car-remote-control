#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <stdio.h>
#include <stdlib.h>
#include "ESPAsyncWebServer.h"
#include "./page.c"
// #include "./video-streaming.c"

#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

const int ledPin = 2;
const int freq = 5000;
const int ledChannel = 0;
// const int resolution = 8;

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

camera_fb_t * fb = NULL;
size_t _jpg_buf_len = 0;
uint8_t * _jpg_buf = NULL;
// char * part_buf[64];
bool imageMemCleaned = true;

// Set these to your desired credentials.
const char *ssid = "my-car";
const char *password = "my-super-car";
struct RespBody {
  size_t size;
  uint8_t * body;
};
RespBody imagePayload[3] =  {
  { size: 0, body: NULL},
  { size: 0, body: NULL},
  { size: strlen(_STREAM_BOUNDARY), body: (uint8_t *)_STREAM_BOUNDARY}
};
RespBody resp;
char part_buf[64];

int phase = 0;
// 0 - stream part, 1 - content, 2 - boundary
int copiedPart = 0;
int handledTimeStart = 0;
const int maxHandledTime = 5000;
// DNS server
const byte DNS_PORT = 53;

DNSServer dnsServer;


AsyncWebServer server(80);

void setup() {
//  pinMode(LED_BUILTIN, OUTPUT);
//  // pinMode(engEnabledPin, OUTPUT);
//  pinMode(engFirstPin, OUTPUT);
//  pinMode(engSecondPin, OUTPUT);

  // ledcSetup(ledChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  // ledcAttachPin(ledPin, ledChannel);
//
//  ledcSetup(servoChannel, servoFreq, servoResolution);
//  ledcAttachPin(servoPin, servoChannel);
//
//  ledcSetup(engChanel, engFreq, engResolution);
//  // attach the channel to the GPIO to be controlled
//  ledcAttachPin(engEnabledPin, engChanel);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  setupCameraServer();
  server.on("/", HTTP_GET, handleRoot);
  server.on("/wheels", HTTP_PUT, handleWheels);
  server.on("/image", HTTP_GET, handleImage);
  server.on("/image-stream", HTTP_GET, handleImageStream);

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
RespBody getImage() {
  fb = esp_camera_fb_get();
    if (!fb) {
     Serial.println("Camera capture failed");
    } else {
      Serial.println("Camera capture not failed");
      Serial.println(fb->width);
        if(fb->format != PIXFORMAT_JPEG){
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if(!jpeg_converted){
            Serial.println("JPEG compression failed");
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
          Serial.println("Image captured");
          Serial.println(_jpg_buf_len);
        }
    }
   imageMemCleaned = false;
   RespBody imageResp;
   imageResp.size = _jpg_buf_len;
   imageResp.body = _jpg_buf;
   return imageResp;
}

void cleanCreatedImage() {
  if (imageMemCleaned) {
    return;
  }
  Serial.println("Cleaning mem");

  if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if(_jpg_buf){
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
  Serial.println("Mem cleaned");
    imageMemCleaned = true;
}

void handleImage(AsyncWebServerRequest *request) {
    RespBody resp = getImage();
    AsyncWebServerResponse * response = request->beginResponse_P(200, "image/jpeg", resp.body, resp.size);
    request->send(response);
    free(resp.body);
    resp.body = NULL;
    cleanCreatedImage();
}
bool payloadInitialized = false;

void cleanBody(RespBody data) {
  if (data.body != NULL) {
    data.body = NULL;
  }
}
void setPayload() {
  cleanBody(imagePayload[0]);
  cleanBody(imagePayload[1]);
  payloadInitialized = true;
  const RespBody image = getImage();
  imagePayload[1].body = image.body;
  imagePayload[1].size = image.size;
  imagePayload[0].size = snprintf((char *)&part_buf, 64, _STREAM_PART, imagePayload[1].size);
  imagePayload[0].body = (uint8_t *)&part_buf;
}
bool allPayloadSent(int phase, int copiedPart) {
  return phase == 0 && copiedPart == 0;
}
int handlePayload(uint8_t *buffer, size_t maxLen) {
  if (!payloadInitialized) {
    return 0;
  }
  RespBody curResp = imagePayload[phase];
  int copiengSize;
  int startFrom;
  const int curPhase = phase;
  if (curResp.size - copiedPart > maxLen) {
    copiengSize = maxLen;
    startFrom = copiedPart;
    copiedPart += maxLen;
  } else {
    copiengSize = curResp.size - copiedPart;
    startFrom = copiedPart;
    copiedPart = 0;
    phase = ++phase % 3;
  }
  Serial.printf("maxLen %u startFrom %u, copiengSize %u, curPhase %u", maxLen, startFrom, copiengSize, curPhase);
  Serial.println();
  memcpy(buffer, curResp.body + startFrom, copiengSize);
  if (phase == 2 && copiedPart == 0) {
    cleanCreatedImage();
  }
  if (allPayloadSent(phase, copiedPart)) {
    payloadInitialized = false;
  }

  return copiengSize;
}

void handleImageStream(AsyncWebServerRequest *request) {
  cleanCreatedImage();
  payloadInitialized = false;
  handledTimeStart = millis();
  AsyncWebServerResponse *response = request->beginChunkedResponse(_STREAM_CONTENT_TYPE, [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
    if (!payloadInitialized) {
      if (handledTimeStart + maxHandledTime < millis()) {
        return 0;
      }
      setPayload();
    }

    return handlePayload(buffer, maxLen);
  });
  request->send(response);
}


void setupCameraServer() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

//  if(psramFound()){
//    config.frame_size = FRAMESIZE_UXGA;
//    config.jpeg_quality = 10;
//    config.fb_count = 2;
//  } else {
    config.frame_size = FRAMESIZE_CIF;
    // config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;
//  }

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  Serial.println("Camera Initialized");
  // Start streaming web server
  // startCameraServer();
}

void loop(void) {
  dnsServer.processNextRequest();
}
