/*Paul Dieterich
 *SOSE2022 
 *IoT / Industrie 4.0
 * 
 */
#include "M5Stack.h"
#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
const char *ssid = "ssid_CAM";
const char *password = "12345678";
void setup() {
  M5.begin();
  M5.Power.begin();
  WiFi.begin(ssid,password);
  Serial.println("Connected To WiFi");
}

void loop() {
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  http.begin("http://192.168.4.1/capture");
  int httpCode = http.GET();
  Serial.println(httpCode);
  if(httpCode > 0){
    if(httpCode == HTTP_CODE_OK){
      int len = http.getSize();
      uint8_t* buff = (uint8_t*) malloc(len);
      WiFiClient* stream = http.getStreamPtr();
      size_t size = stream->available();
      if(size){
        stream->readBytes(buff,len);  
        M5.Lcd.drawJpg(buff,len);     
      }
    free(buff);
    }
  }else{
    Serial.printf("[HTTP] GET.... failed, error: %s\n",http.errorToString(httpCode).c_str());
  }
}
