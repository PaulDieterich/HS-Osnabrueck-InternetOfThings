/**
 * Analog anzeige mit hilfe von TFT_Clock Sketch
 * Digital anzeige mit hilfe von TFT_Clock_Digital Skech 
 */
#include <M5Stack.h>
#include <WiFi.h>
#include "time.h"
#include "NTPClient.h"
#include "WiFiUdp.h"

uint16_t BACKGROUND_COLOR = 0x0000;
uint16_t TEXT_COLOR = 0xFFFF;
uint16_t COLOR = 0xFFFF;
uint16_t X_OFFSET = 100, Y_OFFSET = 100;

const char* ssid = "GalaxyA52";
const char* password ="jwxz2146";
const char* ntpServer = "pool.ntp.org";
bool initial = 1;
bool darkmode = 0;
bool analogClock = 0;
bool digitalClock = 0;
 uint16_t h_sx = 120, h_sy = 120;
  uint16_t h_mx = 120, h_my = 120;
  uint16_t h_hx = 120, h_hy = 120;
  uint16_t sec_deg, min_deg, hou_deg;
  double hou_x, hou_y, min_x,min_y,sec_x,sec_y;
  uint16_t H_POINTER_LENGTH = 75;
  uint16_t M_POINTER_LENGTH = 50;
  uint16_t S_POINTER_LENGTH = 90;
int hours,minutes,seconds;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  //init 
  M5.begin();
  M5.Power.begin();
  //connect to Wifi
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.print(" Connected!");
   //connect to NTP server
   timeClient.begin();
   draw_analog_clock();
   attachInterrupt(GPIO_NUM_39, setDigitalClock ,CHANGE); // Button A
   attachInterrupt(GPIO_NUM_38, setAnalogClock ,CHANGE); // Button B
   attachInterrupt(GPIO_NUM_37, setDarkmode ,CHANGE); // Button C

}
void setDarkmode(){
  if(darkmode){
    darkmode = 0;
  }else{
    darkmode = 1;
  }
}
void setAnalogClock(){
  if(analogClock){
    analogClock = 0;
  }else{
    analogClock = 1;
  }
}
void setDigitalClock(){
  if(digitalClock){
    digitalClock = 0;
  }else{
    digitalClock = 1;
  }
}
int period = 1000;
unsigned long time_now = 0;
void loop() {
      M5.update();
      timeClient.update();
      if(millis() > time_now + period){
        time_now = millis();
         seconds = timeClient.getSeconds();
         minutes = timeClient.getMinutes();
         hours = timeClient.getHours();
          if(darkmode){
            LightSensorData();
          }
          if(analogClock){
            run_analog_clock(seconds,minutes,hours); 
          }
          if(digitalClock){
            run_digital_clock();
          }
        /*
        Serial.println(timeClient.getFormattedTime());        
        String time = timeClient.getFormattedTime();
        char myTime[]= "00:00:00";
        time.toCharArray(myTime,sizeof(myTime));
        char* tokens = strtok(myTime, ":");
        int* timearray = (int*)malloc(sizeof(int) * 3);
        int i = 0;
        while (tokens != NULL) {
          timearray[i] = atoi(tokens);
          tokens = strtok(NULL, ":");
          i++;
        }
        hours = timearray[0] + 2;
        minutes = timearray[1];
        seconds = timearray[2];
          free(timearray);
        */   
         run_analog_clock(seconds,minutes,hours); 
      }
         
}
void LightSensorData(){
  uint32_t pin_data;
  pin_data = analogRead(36);
  Serial.println(pin_data);
  if(pin_data > 2900){
    // ES IST DUNKEL
    M5.Lcd.setBrightness(20);
    //BACKGROUND_COLOR = 0x0000;
    //TEXT_COLOR = 0x8410;
  }
  else if(pin_data < 2000){
    // ES IST HELL
    M5.Lcd.setBrightness(100);
   // BACKGROUND_COLOR = 0x0000;
   // TEXT_COLOR = 0xFFFF;
  }
}

void draw_info(){
  M5.Lcd.fillScreen(BACKGROUND_COLOR);
  M5.Lcd.setCursor(50,100);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
  M5.Lcd.printf("Press (1)- Analog Clock\n");
  M5.Lcd.printf("Press (1- 2 sec)- StopUhr\n");
  M5.Lcd.printf("Press (2)- Digital Clock\n");
  M5.Lcd.printf("Press (3)- Switch \n \t \t \t \t \t \t \t \t \t \t Night Mode");
  M5.Lcd.printf("Press (3- 2 sec) - Info");
}
void StopUhr(){
  
}

void run_digital_clock(){
  M5.Lcd.fillScreen(BACKGROUND_COLOR);
  M5.Lcd.setCursor(50,100);
  M5.Lcd.setTextSize(5);
  M5.Lcd.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
  M5.Lcd.print(timeClient.getFormattedTime());
}

void draw_analog_clock(){
  uint16_t R_OUTER = 100,R_INNER = 90;
  uint16_t pos_x,pos_y;
  uint16_t x,y;
  M5.Lcd.fillCircle(X_OFFSET, Y_OFFSET, R_OUTER, 0x5AFF);
  M5.Lcd.fillCircle(X_OFFSET, Y_OFFSET, R_INNER, 0x5AEB);
  for(int i = 0; i<360; i++) {
    pos_x = cos((i-90) * 0.0174532925);
    pos_y = sin((i-90) * 0.0174532925);
    x = pos_x * ((R_OUTER + R_INNER) / 2) + X_OFFSET;
    y = pos_y * ((R_OUTER + R_INNER) / 2) + Y_OFFSET;
    if(i%90 == 0){
      M5.Lcd.fillCircle(x, y, 5, 0xFFFF);
    }
  }
}

void run_analog_clock(int seconds, int minutes, int hours){
 
 
   Serial.printf("%i - %i - %i\n",hours,minutes,seconds);
    sec_deg = seconds*6;
    min_deg = minutes*6 + sec_deg*0.01666667;
    hou_deg = hours*30 + min_deg*0.0833333;
    hou_x = cos((hou_deg-90)*0.0174532925);
    hou_y = sin((hou_deg-90)*0.0174532925);
    min_x = cos((min_deg-90)*0.0174532925);
    min_y = sin((min_deg-90)*0.0174532925);
    sec_x = cos((sec_deg-90)*0.0174532925);
    sec_y = sin((sec_deg-90)*0.0174532925);
    if (seconds == 0 || initial) {
      M5.Lcd.drawLine(h_hx, h_hy, X_OFFSET, Y_OFFSET, 0x5AEB);
      h_hx = hou_x*H_POINTER_LENGTH + X_OFFSET;
      h_hy = hou_y*H_POINTER_LENGTH + Y_OFFSET;
      M5.Lcd.drawLine(h_mx, h_my, X_OFFSET, Y_OFFSET, 0x5AEB);
      h_mx = min_x*M_POINTER_LENGTH + X_OFFSET;
      h_my = min_y*M_POINTER_LENGTH + Y_OFFSET;
      initial = 0;
    }
    M5.Lcd.drawLine(h_sx, h_sy, X_OFFSET, Y_OFFSET, 0x5AEB);
    h_sx = sec_x*S_POINTER_LENGTH + X_OFFSET;
    h_sy = sec_y*S_POINTER_LENGTH + Y_OFFSET;
    //M5.Lcd.fillCircle(X_OFFSET, Y_OFFSET, RADIUS, 0x5AEB);
    M5.Lcd.drawLine(h_hx, h_hy, X_OFFSET, Y_OFFSET, RED);
    M5.Lcd.drawLine(h_mx, h_my, X_OFFSET, Y_OFFSET, BLUE);
    M5.Lcd.drawLine(h_sx, h_sy, X_OFFSET, Y_OFFSET, GREEN);

    
    //execTime = millis() + 1000;
    

}
