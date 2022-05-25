#include <M5Stack.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>


int dry = 4095;
int wet = 1800;
double soilStatus; 
uint8_t battery;
long lastMsg = 0;
char soilStat_c[300];
WebServer server(80);
WiFiClient wClient;
PubSubClient client(wClient);

//LED
#define NUM_LEDS 10
#define PIN 15
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800); //set number of LEDs, pin number, LED type.  设置灯的数量,控制引脚编号,灯灯类型


//Angabe des verwendeten Brokers
//
const char* mqtt_server = "broker.mqttdashboard.com";

// SSID & Password
const char* ssid = "";
const char* password = "";



const char* CLIENT_ID = "clientId-t3hrcs6GBs";
const char* TOPIC = "home/test";
const char* MESSAGE = "";

const char* callnackTopic = "";
String callbackMessage ="";

void callback(char* callnackTopic, byte* message, unsigned int length){
  Serial.printf("Topic: %c\n ",callnackTopic);
  String messageTemp; 
  for(int i = 0; i < length; i++){
    //Serial.print((char) message[i]);
    messageTemp +=(char)message[i];
  }
  callbackMessage = messageTemp;
  Serial.printf("Message: %c\n", messageTemp);
}

void reconnect(){
  while(!client.connected()){
    Serial.print(CLIENT_ID);
    Serial.print("Attempting MQTT connection....\n");
    if(client.connect(CLIENT_ID)){
      Serial.print("connected");
      client.subscribe(TOPIC);
    }else{
      Serial.print(client.state());
    }
  }
}

void ledBatteryStatus(){
  uint32_t pixelColor;
  if(battery > 75){
    Serial.print("battery über 2/3 \n");
     pixelColor =  pixels.Color(0, 100,0 );
  }else if(battery > 25 && battery <= 57){
    Serial.print("battery über 1/3\n");
    pixelColor = pixels.Color(0,100,100);
  }else if(battery >= 25){
    Serial.print("battery unter 1/3\n");
    pixelColor =  pixels.Color(100, 0, 0);
  }
  for(int i = 0; i < 4; i++){
    pixels.setPixelColor(i,pixelColor );
  }
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
// Handle root url (/)

void handleRoot(){
  // HTML & CSS contents which display on web server
String HTML = "<!DOCTYPE html><html><body><h1> M5 stack soil station</h1><p> soilstatus: ";
       HTML += soilStat_c;
       HTML += "</p><p> battery status: ";
       HTML += battery;
       HTML += "</p>";
       HTML += "<p> nachricht von anderem gerät: ";
       HTML += callbackMessage;
       HTML += "</p></body></html>";
  server.send(200, "text/html", HTML);

}
void ledSoilStatus(){
  uint32_t pixelColor;
  if(soilStatus > 3500){
    pixelColor =  pixels.Color(100, 0, 0);
    Serial.print("Erde ist trocken \n");
  }else if(soilStatus >= 2000 && soilStatus <= 3500){
    pixelColor = pixels.Color(100,100,0);
    Serial.print("Erde wird trocken \n");
  }else if(soilStatus < 2000){
    Serial.print("Erde ist nass \n");
    pixelColor =  pixels.Color(0, 100,0 );
  }
  for(int i = 5; i < 10; i++){
     pixels.setPixelColor(i,pixelColor );
  }
  
}
void setup() {
  M5.begin();
  M5.Power.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  //1883
  client.setServer(mqtt_server, 1883);
  // client.setCallback(callback);
  Serial.println(WiFi.localIP());  //Show ESP32 IP on serial
  M5.Lcd.setTextSize(2);  //Set the text size to 2.  设置文字大小为2
  pinMode(26, INPUT); //Set pin 26 to input mode.  将引脚26设置为输入模式
  dacWrite(25, 0);  //disable the speak noise.  禁用喇叭

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}
void loop() {
  
  if(!client.connected()){
    reconnect();
  }
  client.loop();
  long now = millis();
  if(now - lastMsg > 1000){
    lastMsg = now;
    battery = M5.Power.getBatteryLevel();
    soilStatus = analogRead(36);
    sprintf(soilStat_c,"%f",soilStatus);
    char* MESSAGE = soilStat_c;
    M5.Lcd.setCursor(0, 80);  //Set the cursor at (0,80).  将光标设置在(0,80)
    M5.Lcd.printf("Soil status:%s\n", soilStat_c);
    M5.Lcd.printf("BAT level: %i\n",battery);
    Serial.printf("Message: %s\n", MESSAGE);
    client.publish(TOPIC,MESSAGE);
    ledBatteryStatus();
    ledSoilStatus();  
    pixels.show();
  }
  server.handleClient();  
}
