/* Millisec function */
const int SW1 = 16; // Port SDA1 Pin IO4

/* Reference KidBright Board:
   https://kidbright.info/files/Sch_KidBright32%20V1.3.pdf */

bool buttonStatus_1 = false;
int counter_start = 0;
int counter_stop = 0;
int time_capture = 0;
int State_counter = 0;

void setup() {
  pinMode(SW1, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  buttonStatus_1 = digitalRead(SW1);

  if ((buttonStatus_1 == LOW) && (State_counter == 0)) {
    State_counter = 1;
    counter_start = millis();
  } else if ((buttonStatus_1 == HIGH) && (State_counter == 1)) {
    counter_stop = millis();
    time_capture = counter_stop - counter_start;

    Serial.print("Low level Time capture = ");
    Serial.println(time_capture);

    State_counter = 0;
  }
}
