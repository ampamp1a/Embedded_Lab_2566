int state = LOW;
const int digital_PIN = 19; //Set digita; I/O at pin 19
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer_0() {
  state =! state;
  digitalWrite(digital_PIN, state);
  Serial.println(String("onTimer()") + String(millis()));
}

void setup() {
  Serial.begin(115200);
  pinMode(digital_PIN,OUTPUT);  // set digital I/O at pin 19 to output mode
  Serial.println("start Timer");
  timer = timerBegin(0,80,true); //timerBegin(0 is timer number 0, 80 is prescalers divider, true is count-up timer)
  timerAttachInterrupt(timer, &onTimer_0, true); //timerAttachInterrupt(timer mode, &onTimer_0 call interrupt function , true is rising edge interrupt)
  //This function is used to configure alarm value and autoreload of the timer.
  timeAlarmWrite(timer, 20000, true); //timeAlarmWrite(timer mode, 20000 is value to generate event, true is enable autoreload)
  timerAlarmEnable(timer);
}

void loop(){

}