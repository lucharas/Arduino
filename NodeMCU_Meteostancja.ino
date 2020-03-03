// ***********************************************************
//       STACJA METEO ver 02.03.2020
//
// ***********************************************************


#include <Wire.h>
#include <SPI.h>


//BME280
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme; // I2C

// GY-219 Current Sensor
#include <Adafruit_INA219.h>
Adafruit_INA219 ina219;

#define SW_VERSION " ThinkSpeak.com"

/* ESP12-E & Thinkspeak*/
#include <ESP8266WiFi.h>
WiFiClient client;
const char* MY_SSID = "UPC0413064";
const char* MY_PWD = "b4hnxjyMsssb";
const char* TS_SERVER = "api.thingspeak.com";
String TS_API_KEY ="DXZZX0JYDTGYMBBF";
int sent = 0;

// Zarządzanie czasem
unsigned long previousMillis = 0;  
const long interval = 60000; 

boolean getDataFlag = 1;

//sensors Data
float tem;
float hum;
float pre;
float sol;
float bat;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  delay(10);
  
  bme.begin();
  
  ina219.begin();
  
  connectWifi();
}



void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval - 10000 && getDataFlag == 1){
    getDataFlag = 0;
    getData();
  }
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial.print("T: ");
    Serial.print(tem);
    Serial.print("  H: ");
    Serial.print(hum);
    Serial.print("  P: ");
    Serial.print(pre);
    Serial.print("  SR: ");
    Serial.print(sol);
    Serial.print("  V: ");
    Serial.println(bat);
    getDataFlag = 1;
    sendDataTS();
  }
}

void getData() {
  //BME 
  tem = bme.readTemperature();
  hum = bme.readHumidity(); 
  pre = bme.readPressure() / 100.0F;
  
  //Solar Radiation
  sol = 0.0;
  for (int n = 0; n < 10; n++) {
    sol += ina219.getCurrent_mA();
    delay(5);
  }
  sol /= 10.0;
  if (sol < 0) sol = 0.0;

  //Battery Voltage
  float vol = 0.0;
  for (int n = 0; n < 10; n++) {
    vol += analogRead(A0);
    delay(5);
  }
  vol /= 10.0;
  bat = (float(vol) * 0.0159988);  // dzielnik napięcia
  
  
}

/***************************************************
 * Connecting WiFi
 **************************************************/
void connectWifi() {
  Serial.print("Connecting to "+ *MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("");  
}

/***************************************************
 * Sending Data to Thinkspeak Channel
 **************************************************/
void sendDataTS(void) {
   if (client.connect(TS_SERVER, 80)) { 
     String postStr = TS_API_KEY;
     postStr += "&field1=";
     postStr += String(tem);
     postStr += "&field2=";
     postStr += String(hum);
     postStr += "&field3=";
     postStr += String(pre);
     postStr += "&field4=";
     postStr += String(sol);
     postStr += "&field5=";
     postStr += String(bat);
     
     postStr += "\r\n\r\n";
   
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: " + TS_API_KEY + "\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
     delay(1000); 
   }
   sent++;
   client.stop();
}
