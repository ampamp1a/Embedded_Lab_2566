int state_timer_0 = LOW;
int state_timer_1 = LOW;
int duty_0 = 80;
int duty_1 = 80;

const int digital_PIN_T0 = 19;
const int digital_PIN_T1 = 18;

hw_timer_t * timer_0 = NULL;
hw_timer_t * timer_1 = NULL;

String ReadString;

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer_0(){
  if (state_timer_0 == LOW){
    timerAlarmWrite(timer_0, 100-duty_0, true);
    digitalWrite(digital_PIN_T0, state_timer_0);
    state_timer_0 = HIGH;
  }
  else{
    timerAlarmWrite(timer_0, duty_0, true);
    digitalWrite(digital_PIN_T0, state_timer_0);
    state_timer_0 = LOW;
  }
}

void IRAM_ATTR onTimer_1(){
  if (state_timer_1 == LOW){
    timerAlarmWrite(timer_1, 100-duty_1, true);
    digitalWrite(digital_PIN_T1, state_timer_1);
    state_timer_1 = HIGH;
  }
  else{
    timerAlarmWrite(timer_1, duty_1, true);
    digitalWrite(digital_PIN_T1, state_timer_1);
    state_timer_1 = LOW;
  }
}

void GetDuty(String duty) {
  int channel, duty_cycle;
  
  // Check if the pattern correct
  if (sscanf(duty.c_str(), "Duty %d = %d", &channel, &duty_cycle) == 2) {
    Serial.print("Channel: ");
    Serial.println(channel);
    Serial.print("Duty Cycle: ");
    Serial.println(duty_cycle);

    if (channel == 1) {
      duty_0 = duty_cycle;
    } else if (channel == 2) {
      duty_1 = duty_cycle;
    }
  } else {
    Serial.println("Invalid Format");
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(digital_PIN_T0, OUTPUT);
  pinMode(digital_PIN_T1, OUTPUT);
  Serial.println("Start Timer ");

  timer_0 = timerBegin(0,80,true);
  timer_1 = timerBegin(1,160,true);

  timerAttachInterrupt(timer_0, &onTimer_0, true);
  timerAlarmWrite(timer_0, 50, true);
  timerAlarmEnable(timer_0);

  timerAttachInterrupt(timer_1, &onTimer_1, true);
  timerAlarmWrite(timer_1, 100, true);
  timerAlarmEnable(timer_1);
}

void loop() {
  while(Serial.available()) {  //while loop วนรับค่า โดยจะ Serial.available() เมื่อ Serial ถูกใช้งาน
    ReadString = Serial.readString();  //Serial.readString() สำหรับเก็บค่า String ไว้ในตัวแปร ReadString
    Serial.print("String Read= ");
    Serial.println(ReadString);  // แสดงผลเพื่อเช็คความถูกต้องผ่านทาง Serial
    GetDuty(ReadString);
  }
}