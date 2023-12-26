const int trig = 4; // ประกาศขา trig
const int echo = 5; // ประกาศขา echo
volatile bool echoStatus = false; // ประกาศตัวแปรเก็บสถานะของ echo (volatile เพื่อให้สามารถเข้าถึงได้จาก interrupt)
volatile int State_counter = 0;
volatile int counter_start = 0;
volatile int counter_stop = 0;
volatile int time_capture = 0;

void setup() {
  Serial.begin(115200);
  pinMode(echo, INPUT_PULLDOWN); // สั่งให้ขา echo ใช้งานเป็น input
  pinMode(trig, OUTPUT); // สั่งให้ขา trig ใช้งานเป็น output
  attachInterrupt(digitalPinToInterrupt(echo), echo_change, CHANGE); // ตั้งค่า interrupt ที่ขา echo
}

void echo_change() {
  echoStatus = !echoStatus; // เปลี่ยนสถานะของ echo เมื่อเกิด interrupt
    if ((echoStatus == HIGH) && (State_counter == 0 )) {
    counter_start = micros();
    State_counter = 1;   
  } 
  else if ((echoStatus == LOW) && (State_counter == 1 )) {
    counter_stop = micros();
    time_capture = counter_stop - counter_start;
    unsigned long distance = (time_capture / 2) / 29.1; // คำนวณเป็น centimeters

    Serial.print(distance);
    Serial.print(" cm\n");
    State_counter = 0;
  }
}

void loop() {
  digitalWrite(trig, LOW);
  delayMicroseconds(5);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW); // ใช้งานขา trig
  
  delay(1000);

}