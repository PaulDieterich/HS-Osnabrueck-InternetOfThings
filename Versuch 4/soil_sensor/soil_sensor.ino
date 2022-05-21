#include <M5Stack.h>
#include <WiFi.h>
#include <WebServer.h>



int dry = 4095;
int wet = 1800;
double soilStatus; 
uint8_t battery;

WebServer server(80);
WiFiClient wClient;
PubSubClient client(wClient);
//Angabe des verwendeten Brokers
const char* mqtt_server = "broker.mqttdashboard.com";
// SSID & Password
//const char* ssid = "GalaxyA52";
const char* ssid = "DieBuchseDerPandora";
//const char* password = "jwxz2146";
const char* password = "15469492109050310829";

const char* CLIENT_ID = "clientId-CWdww5KOPs";
const char* TOPIC = "testtopic/1"
const char* MESSAGE = "";

void setup() {
  M5.begin(); //Init M5Stack.  初始化M5Stack
  M5.Power.begin(); //Init power  初始化电源模块
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println(WiFi.localIP());  //Show ESP32 IP on serial
  M5.lcd.setTextSize(2);  //Set the text size to 2.  设置文字大小为2
  pinMode(26, INPUT); //Set pin 26 to input mode.  将引脚26设置为输入模式
  dacWrite(25, 0);  //disable the speak noise.  禁用喇叭
}

void loop() {
  M5.Lcd.setCursor(0, 80);  //Set the cursor at (0,80).  将光标设置在(0,80)
  soilStatus = analogRead(36);
  server.handleClient();
  battery = M5.Power.getBatteryLevel();
  M5.Lcd.printf("Soil status:%d\n", analogRead(36));
  M5.Lcd.printf("BAT level: %i\n",battery);
  ledBatteryStatus();
  ledSoilStatus();
  
  MESSAGE += "{soilStatus";
  MESSAGE += soilStatus;
  MESSAGE += ", batteryStatus ";
  MESSAGE += battery;
  MESSAGE += "}";
  
  if(!client.connected(){
    while(!client.connected()){
      if(client.connect(CLIENT_ID){
        client.subscribe(TOPIC);
      }
    }
  }
  
  client.loop();
  client.publish(TOPIC,MESSAGE);
  
  delay(1000);
}


void ledSoilStatus(){
  if(soilStatus > 3500){
    //Dry
  }else if(soilStatus >= 2000 && soilStatus <= 3500){
    //ok
  }else if(soilStatus < 2000){
    //wett
  }
}

void ledBatteryStatus(){
  if(battery > 75){
    
  }else if(battery > 25 && battery <= 57){
    
  }else if(battery >= 25){
    
  }
}

// HTML & CSS contents which display on web server
String HTML = "<!DOCTYPE html>\
<html>\
<body>\
<h1> M5 stack soil station</h1>\
<p> soilstatus </p>\
</body>\
</html>";

// Handle root url (/)
void handle_root() {
  server.send(200, "text/html", HTML);
}
