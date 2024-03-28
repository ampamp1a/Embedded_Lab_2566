#include <Wire.h>
#define I2C_SDA 4
#define I2C_SCL 5
#define I2C_ADDR 0x6F
#include <Servo.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#include <TridentTD_LineNotify.h>
#define LINE_TOKEN "BDbMBLC8gL3LtxNYrQuHtuTO2QHZD02W1La4xRgbB2G" 

#include <WiFi.h>
#include <PubSubClient.h>
const char *ssid = "Model"; 
const char *password = "Model@12345";
//const char *ssid = "narest_2.4G"; 
//const char *password = "narest6552";

const char *mqtt_broker = "broker.hivemq.com"; 
const char *topic_set = "test_time";
const char *topic_feednow = "feed_now_amp_bew";
const char *topic_temp = "temp_amp_bew";
const char *topic_pressure = "pressure_amp_bew";
const char *topic_time = "time_amp_bew";

//keep for sent to mqtt
char tempkeep[10];
char pressurekeep[10];
char timenow[60];
int hournow;
int minnow;
String timeunit;

//for communicate with mqtt
const int mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);

//receive message from mqtt
String message;
String message2;
//knob
static const int servoPin (22);
Servo servo1;
int posDegrees = 0;

//bmp280
#define BMP_SCK  (18)
#define BMP_MISO (23)
#define BMP_MOSI (21)
#define BMP_CS   (19)
Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

//For feed now
#define mannualfeed (14)

//set time to feed
int c_hour ;
int c_minute;
int c_sec;
int set_hour;
int set_minute;
int set_sec;

String day_week[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday","Sunday"};
String month_name[12] = {"January","Febuary","March","April","May","June","July","August","September","October","November","December"};

char Realtime[60];
int output[7];
String parts[3];

byte decToBcd(byte val) {
  return ((val / 10) << 4) | (val % 10);
}
byte bcdToDec(byte b) {
  return ((b >> 4) * 10) + (b & 0x0F);
}

void setDayAndStart(
  int day_w,
  int day_no,
  int month,
  int year,
  int hour,
  int minute,
  int second
  ) {
      Wire.beginTransmission(I2C_ADDR);
      Wire.write(0); // select 00h second register
      Wire.write((decToBcd(second) & 0x7f) | 0x80);
      Wire.write(decToBcd(minute) & 0x7f);
      Wire.write(decToBcd(hour) & 0x3f);
      Wire.write(decToBcd(day_w) & 0x07);
      Wire.write(decToBcd(day_no) & 0x3f);
      Wire.write(decToBcd(month) & 0x1f);
      Wire.write(decToBcd(year));
      Wire.endTransmission();
    }

void Showday() {
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0);  // select 00h second register
    Wire.endTransmission();
    Wire.requestFrom(I2C_ADDR, 7);
    int *second, *minute, *hour, *day_w, *day_no, *month, *year;
    *second     = bcdToDec(Wire.read() & 0x7f);
    *minute     = bcdToDec(Wire.read() & 0x7f);
    *hour       = bcdToDec(Wire.read() & 0x3f);
    *day_w      = bcdToDec(Wire.read() & 0x07);
    *day_no     = bcdToDec(Wire.read() & 0x3f);
    *month      = bcdToDec(Wire.read() & 0x1f);
    *year       = Wire.read() ;
    sprintf(Realtime, "%s %d/%s/%d %d:%d:%d", day_week[*day_w - 1].c_str(), *day_no, month_name[*month-1].c_str(), bcdToDec(*year), *hour, *minute, *second);
    c_hour = *hour;
    c_minute = *minute;
    c_sec = *second;
    Serial.println(Realtime);
    changetime();
    sprintf(timenow, "%s %d/%s/%d %d:%d:%d %s", day_week[*day_w - 1].c_str(), *day_no, month_name[*month-1].c_str(), bcdToDec(*year), hournow, minnow,*second,timeunit);
    client.publish(topic_time, timenow);
  }
void changetime(){
    if (c_hour >= 12){
        timeunit = "PM";
        hournow = c_hour % 12;
        minnow = c_minute;
    }
    else{
      timeunit = "AM";
      hournow = c_hour;
      minnow = c_minute;
    }
    client.publish(topic_time, timenow);
  }

// motor feed
void knob(){
    for(int posDegrees = 0; posDegrees <= 45; posDegrees++) {
      servo1.write(posDegrees);
      delay(20);
    }
    delay(3000);
    posDegrees = 0;    
    for(int posDegrees = 45; posDegrees >= 0; posDegrees--) {
      servo1.write(posDegrees);
      delay(20);
    }
    LINE.notify("ให้อาหารเรียบร้อย");
  }

void ReadTempandSent(){
  Serial.print(F("Temperature = "));
  float tempfloat = bmp.readTemperature();
  sprintf(tempkeep, "%.2f", tempfloat);
  Serial.print(tempkeep);
  Serial.println(" *C");
  client.publish(topic_temp, tempkeep);

  if(tempfloat > 30){
    LINE.notify("อากาศร้อน น้องมอมแมมร้อนแล้วค้าบ" + String(tempfloat, 2) + "C");
  }

  Serial.print(F("Pressure = "));
  float pressurefloat = bmp.readPressure();
  sprintf(pressurekeep,"%.2f",pressurefloat);
  Serial.print(pressurekeep);
  Serial.println(" Pa");
  client.publish(topic_pressure, pressurekeep);

  Serial.print(F("Approx altitude = "));
  Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
  Serial.println(" m");  
}
void set_feed() {
  int index = 0;
  while (message.indexOf(".") > 0) {
    int pos = message.indexOf(".");
    parts[index] = message.substring(0, pos);
    message = message.substring(pos + 1);
    index++;
  }
  parts[index] = message;
  set_hour = parts[0].toInt();
  set_minute = parts[1].toInt();
  set_sec = parts[2].toInt();
}
//recieve time for set to feed from mqtt
void callback(char *topic_for_set, byte *payload, unsigned int length) {
  message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  if (strcmp(topic_for_set,"test_time")== 0){
      Serial.println(message);
      set_feed();//set time to feed
  }
  else if(strcmp(topic_for_set,"feed_now_amp_bew")== 0){
   Serial.println(message);
    if (message = "feed"){
      knob();
    }   
  }

}

void connectWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
}

void connect_mq() {
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  //client.setCallback(callback2);
  while (!client.connected()) {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());

    Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());

    if (client.connect(client_id.c_str())) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  //client.publish(topic_temp, "connected already");
  client.subscribe(topic_set);
  client.subscribe(topic_feednow);
}

void setup() {
  Wire.begin(4, 5);
  Serial.begin(9600);

  setDayAndStart(7,31,12,23,23,50,50);//Sunday,31,December,23,23,50,50

  servo1.attach(servoPin);//connect Pin for sent signal to servo motor

  //connect to BMP280 sensor
  while ( !Serial ) delay(100);   // wait for native usb
  Serial.println(F("BMP280 test"));
  unsigned status;
  status = bmp.begin(0x76);
  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */  
  //connect mqtt
  connectWiFi();
  connect_mq(); 
  LINE.setToken(LINE_TOKEN);
}

void loop() {
  //feed now when put switch2
  if( digitalRead(mannualfeed) == LOW){
    Serial.println("Feed now");
    knob();
    Serial.println("EAT");
  }  
  client.loop(); //call mqtt always
  Showday();

  //check time for feed
  if ( c_hour == set_hour && c_minute == set_minute && c_sec == 0) {
    knob();
    Serial.println("EAT");
  }

  delay(1000);
  
  ReadTempandSent();//call read temp

  Serial.println();
}