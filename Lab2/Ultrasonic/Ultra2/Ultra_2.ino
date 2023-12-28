const int trig = 4; // ประกาศขา trig
const int echo_1 = 5; // ประกาศขา echo
const int echo_2 = 18; // ประกาศขา echo
int State_counter_1 = 0;
int State_counter_2 = 0;
int counter_start_1 = 0;
int counter_start_2 = 0;
int counter_stop_1 = 0;
int counter_stop_2 = 0;
int timecapture_1 = 0;
int timecapture_2 = 0;
int distance_1 = 0;
int distance_2 = 0;
volatile bool echoStatus_1 = false; // ประกาศตัวแปรเก็บสถานะของ echo (volatile เพื่อให้สามารถเข้าถึงได้จาก interrupt)
volatile bool echoStatus_2 = false; // ประกาศตัวแปรเก็บสถานะของ echo (volatile เพื่อให้สามารถเข้าถึงได้จาก interrupt)

void setup() {
  Serial.begin(115200);
  pinMode(echo_1, INPUT_PULLUP); // สั่งให้ขา echo ใช้งานเป็น input
  pinMode(echo_2, INPUT_PULLUP);
  pinMode(trig, OUTPUT); // สั่งให้ขา trig ใช้งานเป็น output
  attachInterrupt(digitalPinToInterrupt(echo_1), echo_1_change, CHANGE); // ตั้งค่า interrupt ที่ขา echo
  attachInterrupt(digitalPinToInterrupt(echo_2), echo_2_change, CHANGE); // ตั้งค่า interrupt ที่ขา echo
}

void echo_1_change() {
  echoStatus_1 = !echoStatus_1; // เปลี่ยนสถานะของ echo เมื่อเกิด interrupt

  if ((echoStatus_1 == HIGH) && (State_counter_1 == 0)) {
    State_counter_1 = 1;
    counter_start_1 = micros();
  } else if ((echoStatus_1 == LOW) && (State_counter_1 == 1)) {
    counter_stop_1 = micros();
    timecapture_1 = counter_stop_1 - counter_start_1;

    State_counter_1 = 0;
  } 
}

void echo_2_change() {
  echoStatus_2 = !echoStatus_2; // เปลี่ยนสถานะของ echo เมื่อเกิด interrupt
  if ((echoStatus_2 == HIGH) && (State_counter_2 == 0)) {
    State_counter_2 = 1;
    counter_start_2 = micros();
  } else if ((echoStatus_2 == LOW) && (State_counter_2 == 1)) {
    counter_stop_2 = micros();
    timecapture_2 = counter_stop_2 - counter_start_2;

    State_counter_2 = 0;
  } 
}

void loop() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW); // ใช้งานขา trig

  distance_1 = (timecapture_1 / 2) / 29.1;
  Serial.print(distance_1);
  Serial.print(" cm\n");

  distance_2 = (timecapture_2 / 2) / 29.1;
  Serial.print(distance_2);
  Serial.print(" cm\n");

}