#include <Arduino.h>
#define Blue 17
#define Green 12
#define Reset 16
#define Start 14
int led_status = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(Blue, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Reset, INPUT_PULLUP);
  pinMode(Start, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(Reset) == LOW && led_status != 0){

    led_status = 0;
  }

  if(digitalRead(Start) == LOW && digitalRead(Reset) == HIGH ){
    delay(100);

    if(digitalRead(Start) == LOW && digitalRead(Reset) == HIGH ){
      if (led_status == 1){
        led_status = 2;
      }
      else {
        led_status = 1;
        delay(100);
      }
      while (digitalRead(Start) == LOW) {
        delay(10);
      }

    }
  }

  if(led_status==0){ 
    digitalWrite(Green, HIGH);
    digitalWrite(Blue, HIGH);
  }

  if(led_status==1){ 
    digitalWrite(Green, LOW);
    digitalWrite(Blue, HIGH);
  }
  if(led_status==2){ 
    digitalWrite(Green, HIGH);
    digitalWrite(Blue, LOW);

  }
}