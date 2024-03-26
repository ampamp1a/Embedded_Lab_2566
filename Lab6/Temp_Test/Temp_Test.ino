#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <HTTPClient.h>
#define I2C_SDA 4
#define I2C_SCL 5
#define I2C_ADDR 0x4D

// replace with your wifi ssid and wpa2 key
const char *ssid = "narest_2.4G";
const char *pass = "narest6552";

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";

// Service API Key
String apiKey = "GCUGPBJNMAQOPI31"; //Write API Key

float Temp_value ;

WiFiClient client;
Adafruit_8x16minimatrix matrix = Adafruit_8x16minimatrix();

float read_temperature_LM73(void) {
  byte Temp_data[2];
  byte Temp_data_maske[2];
  float Temp_value;
  // I2C start and write slave address (0x4D)
  Wire1.beginTransmission(I2C_ADDR);
  Wire1.write(0); // write temperature address
  Wire1.endTransmission();

  // I2C start andread temperature data two byte
  Wire1.requestFrom(0x4D, 2 );
  Temp_data[1] = Wire1.read(); // receive MSB temp data
  Temp_data[0] = Wire1.read(); // receive LSB temp data

  // check Temperature under 0 degree
  if ((Temp_data[1] & 0x80) == 1) {
  Temp_data_maske[1] = ~Temp_data[1]; // 2'complement
  Temp_data_maske[0] = (~Temp_data[0] + 1) >> 5;
  Temp_value = ((Temp_data_maske[1] * 2) + (Temp_data_maske[0] * 0.25)) * -1; //Temperature value in C degree
  }
  else {
  Temp_data_maske[1] = Temp_data[1] & 0b01111111;
  Temp_data_maske[0] = Temp_data[0] & 0b11100000;
  Temp_data_maske[0] = Temp_data[0] >> 5;
  Temp_value = (Temp_data_maske[1] * 2) + (Temp_data_maske[0] * 0.25); //Temperature value in C degree
  }

  // print MSB temp data in HEX
  Serial.print("Temp_data[1](MSB) :");
  Serial.println(Temp_data[1], HEX);
  // print LSB temp data in HEX
  Serial.print("Temp_data[0] (MSB) :");
  Serial.println(Temp_data[0], HEX);
  Serial.println("////////////////////////////////////////");
  //Temperature value in C degree
  Serial.print("Temp in C :");
  Serial.println(Temp_value);
  Serial.println("...........................");
  return Temp_value;
}
void show_temperature(int temp) {
  matrix.setTextColor(LED_ON);
  matrix.setTextSize(1);
  matrix.setRotation(1);
  matrix.setTextWrap(false);

  matrix.clear();
  matrix.setCursor(2, 1);
  matrix.print(String(temp));
  matrix.writeDisplay();
  return;
}

void httpPostToThingSpeak(unsigned int fieldNumber,float value) {
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // Data to send with HTTP POST
  String httpRequestData = "api_key=" + apiKey + "&field" +
  String(fieldNumber) + "=" + String(value);
  // Send HTTP POST request
  int httpResponseCode = http.POST(httpRequestData);

  if(httpResponseCode == 200){
    Serial.println("Channel update successful.");
  }else if(httpResponseCode == -1){
    Serial.println("Channel update failed.");
  }

  // Free resources
  http.end();
}

void setup() {
  matrix.begin(0x70); // I2C start and write slave address (0x4D)
  Wire1.begin(I2C_SDA, I2C_SCL);
  Serial.begin (115200);
}
unsigned int HalfSeconds = 0;
void loop()
{
  float temp = read_temperature_LM73();
  int tempCorse = round(temp);
  show_temperature(tempCorse);

  // Check if WiFi is connected
  if (WiFi.status() == WL_CONNECTED) {
    //Post http get to ThingSpeak Every 15 seconds
    if(HalfSeconds >= 30){
      //Reset Timer
      HalfSeconds = 0;
      //Write temp To Field 1
      httpPostToThingSpeak(1,temp);
    }

  }else{//Reconnecting to Router
    Serial.print("Connecting to :");
    Serial.print(ssid);
    Serial.print(" : ");
    WiFi.begin(ssid, pass);//Connect ESP32 to home network
    while (WiFi.status() != WL_CONNECTED) //Wait until Connection is complete
    {
      delay(500);
      Serial.print(".");
    }
  }
  //Update Half seconds every 500 ms
  delay(500);
  HalfSeconds++;
}
