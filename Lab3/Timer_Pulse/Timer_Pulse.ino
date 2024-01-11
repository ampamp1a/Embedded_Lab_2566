int state = LOW;
const int digital_PIN = 19; //Set digita; I/O at pin 19
hw_timer_t * timer_0 = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer_0() {
  state =! state;
  digitalWrite(digital_PIN, state);
  
}

void setup() {
  Serial.begin(115200);
  pinMode(digital_PIN,OUTPUT);  // set digital I/O at pin 19 to output mode
  Serial.println("start Timer");
  timer_0 = timerBegin(0,8,true); //timerBegin(0 is timer number 0, 80 is prescalers divider, true is count-up timer)
  timerAttachInterrupt(timer_0, &onTimer_0, true); //timerAttachInterrupt(timer mode, &onTimer_0 call interrupt function , true is rising edge interrupt)
  //This function is used to configure alarm value and autoreload of the timer.
  timerAlarmWrite(timer_0, 203.252, true); //timeAlarmWrite(timer mode, 20000 is value to generate event, true is enable autoreload)
  timerAlarmEnable(timer_0);
}

void loop(){
  Serial.println(String("onTimer()") + String(millis()));
}

