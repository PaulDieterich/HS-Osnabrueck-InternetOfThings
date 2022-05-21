#include <M5Stack.h>
#include <WiFi.h>
#include <TinyGPS++.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include "utility/MPU9250.h"
#define M5STACK_MPU9250


// InfluxDB 2 server url, e.g. http://192.168.1.48:8086 (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "****"
// InfluxDB 2 server or cloud API authentication token (Use: InfluxDB UI -> Load Data -> Tokens -> <select token>)
#define INFLUXDB_TOKEN "****"
// InfluxDB 2 organization name or id (Use: InfluxDB UI -> Settings -> Profile -> <name under tile> )
#define INFLUXDB_ORG "****"
// InfluxDB 2 bucket name (Use: InfluxDB UI -> Load Data -> Buckets)
#define INFLUXDB_BUCKET "****"

static const double BREMEN_LAT = 53.073635 , BREMEN_LNG = 8.806422;
const char* ssid = "****";
const char* password = "****";

static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
MPU9250 IMU;

HardwareSerial hs(2);
float accX = 0, accY = 0, accZ = 0;
float gX = 0, gY = 0, gZ = 0;
int stas = 0; int HDDP = 0;
float latitude = 0,longitude =0, dist = 0;
int duration = 0;
void drawTable(){
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println(F("Sats HDOP Latitude   Longitude    Date        Time"));
  M5.Lcd.println(F("          (deg)      (deg)                        "));
  M5.Lcd.println(F("--------------------------------------------------"));
  M5.Lcd.printf(    "%i   %i  %f   %f \n",stas,HDDP,latitude,longitude);
  M5.Lcd.println(F("Latitude   Longitude    Dauer     Distanz zum Start"));
  M5.Lcd.println(F("(deg)      (deg)        [s]       [m]              "));
  M5.Lcd.println(F("---------------------------------------------------"));
  M5.Lcd.printf("    %f  %f   %f  %f\n",latitude,longitude,duration,dist);
  M5.Lcd.println(F("x-Acceleration y-Acceleration   z-Acceleration     "));
  M5.Lcd.println(F("(mg)           (mg)             (mg)               "));
  M5.Lcd.println(F("---------------------------------------------------"));
  M5.Lcd.printf("%f      %f        %f\n",accX,accY,accZ);
   M5.Lcd.println(F("x-gyro        y-gyro         z-gyro               "));
  M5.Lcd.println(F("(mg)           (mg)           (mg)                 "));
  M5.Lcd.println(F("---------------------------------------------------"));
  M5.Lcd.printf("%f      %f        %f\n",gX,gY,gZ);
}

void connectToWiFi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { 
        delay(500);
        Serial.print(".");
  }
  Serial.println("WiFi connected");
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (hs.available())
      gps.encode(hs.read());
  } while (millis() - start < ms);
}
void writeData(String tagName, String tagValue, String fieldName, float fieldValue){
  InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);
  Point pointDevice("pdieteri");
  pointDevice.addTag(tagName, tagValue);
  pointDevice.addField(fieldName, fieldValue);
  client.writePoint(pointDevice);
}

//https://docs.m5stack.com/en/api/core/mpu9250
// readAccelData()

void getMovment(){
     if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01){
    IMU.readAccelData(IMU.accelCount);
    
    IMU.getAres();
    
    IMU.ax = (float)IMU.accelCount[0] * IMU.aRes;
    IMU.ay = (float)IMU.accelCount[1] * IMU.aRes;
    IMU.az = (float)IMU.accelCount[2] * IMU.aRes;
    accX = IMU.ax * 1000;
    accY = IMU.ay * 1000;
    accZ = IMU.az * 1000;
  }
}



void setup() {
  M5.begin();
  M5.Power.begin();
  Wire.begin();
  connectToWiFi();
  hs.begin(GPSBaud,SERIAL_8N1,16,17);
  configTzTime("pool.ntp.org", "2.europe.pool.ntp.org");
  
  IMU.initMPU9250();
  //IMU.calibrateMPU9250(IMU.gyroBias, IMU.accelBias);
   
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    connectToWiFi();
  } 
  stas = gps.satellites.value();
  HDDP = gps.hdop.value();
  latitude = gps.location.lat();
  longitude = gps.location.lng();
  dist = gps.distanceBetween(BREMEN_LAT, BREMEN_LNG, latitude, longitude);
 getMovment();
  drawTable();
  writeData("device","m5stack","lat",latitude);
  writeData("device","m5stack","lon",longitude);
  writeData("device","m5stack","uptime",millis());

   /* IMU.ax = (float)IMU.accelBias[0] * IMU.aRes;
    IMU.ay = (float)IMU.accelBias[1] * IMU.aRes;
    IMU.az = (float)IMU.accelBias[2] * IMU.aRes;
    accX = IMU.ax * 1000;
    accY = IMU.ay * 1000;
    accZ = IMU.az * 1000;
    */
     
    Serial.printf("X:%f - Y:%f - Z:%f\n",accX,accY,accZ);


    IMU.readGyroData(IMU.gyroCount);  // Read the x/y/z adc values
    IMU.getGres(); // get Gyro scales saved to "gRes"
    IMU.gx = (float)IMU.gyroCount[0] * IMU.gRes;
    IMU.gy = (float)IMU.gyroCount[1] * IMU.gRes;
    IMU.gz = (float)IMU.gyroCount[2] * IMU.gRes;
    gX = IMU.gx * 1000;
    gY = IMU.gy * 1000;
    gZ = IMU.gz * 1000;

    writeData("device","m5stack","accX",accX);
      writeData("device","m5stack","accY",accY);
      writeData("device","m5stack","accZ",accZ);
       writeData("device","m5stack","gx",gX);
      writeData("device","m5stack","gy",gY);
      writeData("device","m5stack","gz",gZ);
}
/*
  M5.Lcd.printf("\n----------------------------------------------\n");
  printInt(gps.charsProcessed(), true, 6);
  printInt(gps.sentencesWithFix(), true, 10);
  printInt(gps.failedChecksum(), true, 9);
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
  M5.Lcd.println(F("No GPS data received: check wiring"));
  */
