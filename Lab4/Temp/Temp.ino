#include <Wire.h>
#define I2C_SDA 4
#define I2C_SCL 5
#define I2C_ADDR 0x4D

byte Temp_data[3];
byte Temp_data_maske[2];
float Temp_value;
float Temp[2];
String  Checkneg;
String  Checkneg2;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin (115200);
}

void loop() {
  int index = 1;
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0); //write one byte
    Wire.endTransmission();
    Wire.requestFrom(0x4D, 2); //read two byte
      while(Wire.available()) //slave may send less than requested
      {
        if(index == 1){
          Temp_data[1] = Wire.read();
          //Temp_data[1] = Wire.read();
          //Temp_data[1] = 0b11101111;//Wire.read(); //try neg
          Checkneg = String(Temp_data[1],BIN);
          if (Checkneg.length() == 8 && Checkneg[0] =='1'){
              Temp_data[0] = Temp_data[1]<<1;
              Temp_data[0] = Temp_data[0]>>1;
              Temp[1] = (Temp_data[0]*2)-256;
              index = index+1;
          } 
          else{
            Temp[1] = Temp_data[1] *2;
            index = index+1;
          }
          
        }
        else{
          Temp_data[2] = Wire.read();
          Temp_data[3] = Temp_data[2] >> 2;
          Checkneg2 = Temp_data[3];
          if (Checkneg.length() == 8 && Checkneg[0] =='1'){
              Temp[2] = Temp_data[3] *0.03125 - 2;
              index = 1;
          } 
          else{
            Temp[2] = Temp_data[3] *0.03125;
            index = 1;
          }          
 
        }
      }
  Serial.println(Temp_data[1],BIN);
  Serial.println(Temp_data[0],BIN);
  Serial.println(Checkneg[0]);

  Serial.println(Temp_data[2],BIN);
  Serial.println(Temp_data[3],BIN);
  

  Serial.print("Temp[1] :");
  Serial.println(Temp[1]);

  Serial.print("Temp[2] :");
  Serial.println(Temp[2]);

  Temp_value = Temp[1] + Temp[2];
  Serial.println(Temp_value);
  Serial.println("..........");
  delay(1000);
}