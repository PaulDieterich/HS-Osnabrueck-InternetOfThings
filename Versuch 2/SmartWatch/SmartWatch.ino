/**
 * Analog anzeige mit hilfe von TFT_Clock Sketch
 * Digital anzeige mit hilfe von TFT_Clock_Digital Skech 
 */
#include <M5Stack.h>
#include <WiFi.h>
#include "time.h"
#include "NTPClient.h"
#include "WiFiUdp.h"
#include "Timer.h"

struct nightDesign{
  uint16_t background = 0x0000; //schwarz
  uint16_t text_color = 0xFFFF; //weiß
  uint16_t color;
  uint16_t h = BLUE;
  uint16_t m = RED;
  uint16_t s = GREEN;
}nightDesign;
struct defaultDesign{
  uint16_t background = 0xFFFF; //weiß
  uint16_t text_color = 0x0000; //schwarz
  uint16_t color;
  uint16_t h = BLUE;
  uint16_t m = RED;
  uint16_t s = GREEN;
}defaultDesign;
  
uint16_t BACKGROUND_COLOR = 0x0000;
uint16_t TEXT_COLOR = 0xFFFF;
uint16_t COLOR = 0xFFFF;


uint16_t X_OFFSET = 150, Y_OFFSET = 100;

const char* ssid = "GalaxyA52";
const char* password ="jwxz2146";
const char* ntpServer = "pool.ntp.org";
bool initial = 1;

bool darkmode = true;
bool analogState = false;
bool digitalState = false;
bool analogInit = false;
bool stopUhrState = false;

uint16_t h_sx = 120, h_sy = 120;
uint16_t h_mx = 120, h_my = 120;
uint16_t h_hx = 120, h_hy = 120;

uint16_t sec_deg, min_deg, hou_deg;

double hou_x, hou_y;
double min_x,min_y;
double sec_x,sec_y;

uint16_t H_POINTER_LENGTH = 75;
uint16_t M_POINTER_LENGTH = 50;
uint16_t S_POINTER_LENGTH = 90;

int hours,minutes,seconds;

int StopUhrSec = 0;
int period = 1000;
unsigned long time_now = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
Timer timer;
void setDarkmode(){
  M5.Speaker.tone(661, 100);    //Set the speaker to tone at 661Hz for 1000ms
  if(darkmode){
    darkmode = false;
    analogInit = false;
    M5.Lcd.clear();
  }else{
    darkmode = true;
    analogInit = false;
    M5.Lcd.clear();
  }
  
}
void setAnalogClock(){
  M5.Speaker.tone(661, 100);    //Set the speaker to tone at 661Hz for 1000ms
   if(analogState == false){
   // digitalState = false;
    analogState = true;
      M5.Lcd.clear();
   }else{
    analogState = false;
      M5.Lcd.clear();
   } 
}

void setDigitalClock(){
    Serial.printf("\n setDigitalClock call \n");
    //M5.Speaker.tone(661, 100);    //Set the speaker to tone at 661Hz for 1000ms
    if(digitalState == false){
      digitalState = true;
      //analogState = false;
      M5.Lcd.clear();
   }else{
    digitalState == false;
    M5.Lcd.clear();
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
  M5.Lcd.fillScreen(darkmode ? nightDesign.background : defaultDesign.background );
  M5.Lcd.setCursor(50,100);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(darkmode ? nightDesign.text_color : defaultDesign.text_color, darkmode ? nightDesign.background : defaultDesign.background);
  M5.Lcd.printf("Press (1)- Analog Clock\n");
  M5.Lcd.printf("Press (1- 2 sec)- StopUhr\n");
  M5.Lcd.printf("Press (2)- Digital Clock\n");
  M5.Lcd.printf("Press (3)- Switch \n \t \t \t \t \t \t \t \t \t \t Night Mode");
  M5.Lcd.printf("Press (3- 2 sec) - Info");
}
void StopUhr(){
    if(millis() > time_now + period){
      time_now = millis();
      timer.incrementTimer();
      M5.Lcd.setCursor(75,150);
      M5.Lcd.setTextSize(2);
      M5.Lcd.setTextColor(darkmode ? nightDesign.text_color : defaultDesign.text_color , darkmode ? nightDesign.background : defaultDesign.background );
      M5.Lcd.printf("%i:%i:%i", timer.getHours(),timer.getMinutes(), timer.getSeconds());
    }  
}

void run_digital_clock(){
  analogInit = false;
  M5.Lcd.fillScreen(darkmode ? nightDesign.background : defaultDesign.background );
  M5.Lcd.setCursor(50,100);
  M5.Lcd.setTextSize(5);
  M5.Lcd.setTextColor(darkmode ? nightDesign.text_color : defaultDesign.text_color , darkmode ? nightDesign.background : defaultDesign.background );
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

void run_analog_clock(){
    seconds = timeClient.getSeconds();
    minutes = timeClient.getMinutes();
    hours = timeClient.getHours() +2;
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
      //studne
      M5.Lcd.drawLine(h_hx, h_hy, X_OFFSET, Y_OFFSET, 0x5AEB);
      h_hx = hou_x*H_POINTER_LENGTH + X_OFFSET;
      h_hy = hou_y*H_POINTER_LENGTH + Y_OFFSET;
      //minutes
      M5.Lcd.drawLine(h_mx, h_my, X_OFFSET, Y_OFFSET, 0x5AEB);
      h_mx = min_x*M_POINTER_LENGTH + X_OFFSET;
      h_my = min_y*M_POINTER_LENGTH + Y_OFFSET;
      initial = 0;
    }
    
    M5.Lcd.drawLine(h_sx, h_sy, X_OFFSET, Y_OFFSET, 0x5AEB);
    h_sx = sec_x*S_POINTER_LENGTH + X_OFFSET;
    h_sy = sec_y*S_POINTER_LENGTH + Y_OFFSET;
    //M5.Lcd.fillCircle(X_OFFSET, Y_OFFSET, RADIUS, 0x5AEB);
    M5.Lcd.drawLine(h_hx, h_hy, X_OFFSET, Y_OFFSET, darkmode ? nightDesign.h : defaultDesign.h);
    M5.Lcd.drawLine(h_mx, h_my, X_OFFSET, Y_OFFSET, darkmode ? nightDesign.m : defaultDesign.m);
    M5.Lcd.drawLine(h_sx, h_sy, X_OFFSET, Y_OFFSET, darkmode ? nightDesign.s : defaultDesign.s);

    //execTime = millis() + 1000;
    
}
void analog_init(){
   if(analogInit == false){
      draw_analog_clock();
      run_analog_clock();
      analogInit = true;
    }else{
      run_analog_clock(); 
    }
}
void setup() {
  //init 
  M5.begin();
  M5.Power.begin();
  M5.Speaker.begin(); //Initialize the speaker
  //connect to Wifi
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.print(" Connected!");
   //connect to NTP server
   timeClient.begin();

   attachInterrupt(GPIO_NUM_39, setDigitalClock ,RISING); // Button A
   attachInterrupt(GPIO_NUM_38, setAnalogClock ,RISING); // Button B
   attachInterrupt(GPIO_NUM_37, setDarkmode ,RISING); // Button C

}

void loop() {
  M5.update(); 
  timeClient.update();
  if(millis() > time_now + period){
    time_now = millis();
    
    Serial.printf("- %i - %i - %i\n", seconds, minutes,hours);
    Serial.printf("A -> digital mode: %d -",digitalState);
    Serial.printf(" B -> analog mode: %d  -", analogState);
    Serial.printf(" C -> darkmode: %d\n",darkmode);
    if(analogState){
       analog_init();  
    }
    if(digitalState){
      run_digital_clock();
    }
    //if(stopUhrState){
    //  run_digital_clock();
      
    //}
    //if (M5.BtnA.pressedFor(2000)) {
    //  StopUhr();
    //}
    
  }
}


//-------------------------------
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
