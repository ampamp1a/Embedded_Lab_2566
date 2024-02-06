#include <Wire.h>
#define I2C_SDA 4
#define I2C_SCL 5
#define I2C_ADDR 0x4D

byte Temp_data[4];  //Temp data from read
float Temp_value;   //temp in degree Celsius
float Temp[2];      //High byte & Low byte
String  Checkneg;   // For negative Temp 

void setup() {  
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin (115200);
}
//
void loop() {
  int index = 1;
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0); //write one byte
    Wire.endTransmission();
    Wire.requestFrom(0x4D, 2); //read two byte
      while(Wire.available()) //slave may send less than requested
      {
        if(index == 1){ // first byte
          Temp_data[1] = Wire.read(); //  High byte
          //Temp_data[0] = Wire.read(); //try neg (dummy)
          //Temp_data[1] = 0b11110111; //try neg 16 ๐C
          //Temp_data[1] = 0b01101111; //try 222๐C
          Checkneg = String(Temp_data[1],BIN);
          if (Checkneg.length() == 8 && Checkneg[0] =='1'){ // Cheg Negative
              Temp_data[0] = Temp_data[1]<<1; // delete sign bit by shift left
              Temp_data[0] = Temp_data[0]>>1; // add 0 at left side by shift right
              Temp[1] = (Temp_data[0]*2)-254; // Convert values to degrees Celsius
              index = index+1; // to next byte
          } 
          else{ //positive
            Temp[1] = Temp_data[1] *2; // Convert values to degrees Celsius
            index = index+1; // to next byte
          }
          
        }
        else{ // second byte
          Temp_data[2] = Wire.read();
          //Temp_data[4] = Wire.read(); //try neg (dummy)
          //Temp_data[2] = 0b10111100; //try neg -0.5 ๐C
          //Temp_data[2] = 0b11101111; //try 1.84๐C
          Temp_data[3] = Temp_data[2] >> 2; // Delete bit D1&D0
          if (Checkneg.length() == 8 && Checkneg[0] =='1'){ // Cheg Negative
              Temp[2] = (Temp_data[3] *0.03125 - 1.96875); // Convert values to degrees Celsius
              index = 1; // to read new temp
          } 
          else{
            Temp[2] = Temp_data[3] *0.03125; // Convert values to degrees Celsius
            index = 1; // to read new temp
          }          
 
        }
      }
  Serial.println("High byte");
  Serial.print("Read :");
  Serial.println(Temp_data[1],BIN); // Read High byte from LM73-1
  Serial.println(Temp_data[0],BIN);
  Serial.print("Degrees Celsius :");
  Serial.println(Temp[1]); // High byte in Degrees Celsius

  Serial.println("Low byte");
  Serial.print("Read :");
  Serial.println(Temp_data[2],BIN); // Read Low byte from LM73-1
  Serial.print("Degrees Celsius :");
  Serial.println(Temp[2]); // Low byte in Degrees Celsius  

  Temp_value = Temp[1] + Temp[2]; // Sum High byte & Low byte
  Serial.print("Total :");
  Serial.print(Temp_value); // Total Temp
  Serial.println(" degrees Celsius");
  Serial.println("..........");
  delay(1000);
}