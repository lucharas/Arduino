// Temperatura
// CJMCU -9808 zestaw
// MCP9808 scalak

#include <Wire.h>

int MCP9808_addr = 0x18;

void setup() {
  Wire.begin();  
  Serial.begin(9600);    

  Wire.beginTransmission(MCP9808_addr);  

}

void loop() {
  float temperature =   MCP9808_Read(MCP9808_addr);
  Serial.println(temperature,1);
  delay(500);
}

float MCP9808_Read(int addr){
  unsigned int data[2];    
  Wire.beginTransmission(addr);  
  Wire.write(0x05);  
  Wire.endTransmission();    
  
  Wire.requestFrom(addr, 2);    
  if(Wire.available() == 2)  {    
    data[0] = Wire.read();    
    data[1] = Wire.read();  
  }    
  int temp = ((data[0] & 0x1F) * 256 + data[1]);  
  if(temp > 4095)  temp -= 8192; 
   
  float cTemp = temp * 0.0625;  

  return cTemp;
}

void MCP9808_Init(int addr) {
  Wire.beginTransmission(addr); 
  Wire.write(0x01);  
  Wire.write(0x00);  
  Wire.write(0x00);
  Wire.endTransmission(); 

  Wire.beginTransmission(addr); 
  Wire.write(0x08);   // Select resolution register 
  Wire.write(0x03);   // Resolution = +0.0625 / C
  Wire.endTransmission();
}

