#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <TridentTD_LineNotify.h>
#define LM73_ADDR 0x4D6
#define SECRET_WRITE_APIKEY "XB8CO61PPXNH4Y1P"
#define LM73_ADDR 0x4D
#define LINE_TOKEN "BDbMBLC8gL3LtxNYrQuHtuTO2QHZD02W1La4xRgbB2G"  // ป้อนรหัส token ที่คัดลอกไว้
// #define WIFI_NAME "TIPARNAN 2.4G"        // ป้อนชื่อ WiFi ที่ต้องการเชื่อมต่อ
// #define WIFI_PASSWORD "Tip3456N"    // ป้อนรหัส WiFi ที่ต้องการเชื่อมต่อ
// #define WIFI_NAME "Hi"        // ป้อนชื่อ WiFi ที่ต้องการเชื่อมต่อ
// #define WIFI_PASSWORD "nathamon"    // ป้อนรหัส WiFi ที่ต้องการเชื่อมต่อ
#define WIFI_NAME "yaikaew"        // ป้อนชื่อ WiFi ที่ต้องการเชื่อมต่อ
#define WIFI_PASSWORD "123456789"    // ป้อนรหัส WiFi ที่ต้องการเชื่อมต่อ
int analog_value = 0;
double temp = 0;
String server = "api.thingspeak.com";
float readTemperature() {
  Wire1.beginTransmission(LM73_ADDR);
  Wire1.write(0x00);
  Wire1.endTransmission();
  uint8_t count = Wire1.requestFrom(LM73_ADDR, 2);
  float temp = 0.0;
  if (count == 2) {
    byte buff[2];
    buff[0] = Wire1.read();
    buff[1] = Wire1.read();
    temp += (int)(buff[0] << 1);
    if (buff[1] & 0b10000000) temp += 1.0;
    if (buff[1] & 0b01000000) temp += 0.5;
    if (buff[1] & 0b00100000) temp += 0.25;
    if (buff[0] & 0b10000000) temp *= -1.0;
  }
  return temp;
}
void setup() {
  Serial.begin(115200);
  Wire1.begin(4, 5);
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
  Serial.printf("WiFi connecting ", WIFI_NAME);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  LINE.setToken(LINE_TOKEN);
}

void loop() {
  delay(5000);
  float t = readTemperature();
  Serial.print("\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  SendWriteRequestToThingSpeak(t);
  if (t < 30.0)  // หากค่าอุณหภูมิน้อยกว่า30องศา LINE Notify จะแจ้งเตือนข้อความ
  {
    LINE.notify("อากาศเย็นสบาย" + String(t, 3) + "C");
  }
  if (t >= 30.0)  //หากค่าอุณหภูมิมากกว่าเท่ากับ30องศา LINE Notify จะแจ้งเตือนข้อความ
  {
    LINE.notify("อากาศร้อน" + String(t, 3) + "C");
  }
  delay(15000);
}
void SendWriteRequestToThingSpeak(float t) {
  HTTPClient https;
  //Prepare Http
  String STR_HTTP = "https://" + server + "/update?api_key=";
  STR_HTTP += SECRET_WRITE_APIKEY;
  STR_HTTP += "&field1=";
  STR_HTTP += String(t);
  //send http request
  if (https.begin(STR_HTTP)) {  // HTTPS
    Serial.print("[HTTPS] GET...\n");
    // start connection and send HTTP header
    int httpCode = https.GET();
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n",
                      httpCode);
      if (httpCode == 200) {
        Serial.println("Channel update successful.");
      } else {
        Serial.println("Problem updating channel. HTTPerror code " + String(httpCode));
      }
    } else {
      Serial.printf("[HTTPS] GET... failed, error:%s\n", https.errorToString(httpCode).c_str());
    }
    https.end();
  } else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }
}