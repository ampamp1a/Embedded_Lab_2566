#include <Wire.h>
#define I2C_SDA 4
#define I2C_SCL 5
#define I2C_ADDR 0x6F
int i = 0;

String day_week[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday","Sunday"};
String month_name[12] = {"January","Febuary","March","April","May","June","July","August","September","October","November","December"};

char Realtime[60];
int output[7];
String input;

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


void Showday(
) {
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0);  // select 00h second register
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, 7);
  int *second, *minute, *hour, *day_w, *day_no, *month, *year;
  *second     = bcdToDec(Wire.read() & 0x7f);
  *minute     = bcdToDec(Wire.read() & 0x7f);
  *hour       = bcdToDec(Wire.read() & 0x3f);
  *day_w       = bcdToDec(Wire.read() & 0x07);
  *day_no       = bcdToDec(Wire.read() & 0x3f);
  *month      = bcdToDec(Wire.read() & 0x1f);
  *year       = Wire.read() ;
  sprintf(Realtime, "%s %d/%s/%d %d:%d:%d", day_week[*day_w - 1].c_str(), *day_no, month_name[*month-1].c_str(), bcdToDec(*year), *hour, *minute, *second);
  Serial.println(*year);
  Serial.println(Realtime);
}

void setup() {
  Wire.begin(4, 5);
  Serial.begin(115200);
  setDayAndStart(7,31,12,23,23,59,50);//Sunday,31,December,23,23,59,50
  // Serial.println("fomate time set = <day_w, day_no, month, year, hour, minute, second>");
}

void loop() {
  Showday();
  delay(1000);
}