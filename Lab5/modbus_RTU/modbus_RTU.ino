const int slave_addr = 1;   // Slave Address of Modbus Device
const int dirpin = 18;      // Direction Control Pin 
uint8_t buf[128];
int8_t indexMax = 0;
int8_t m_error = 0;
unsigned long prev_t;
                                            /* packet with CRC16 */
                                          /*CRC(16) Calculator  https://crccalc.com/ */
          //          address   function code   data address 2 byte   data length 2 byte     CRC2 byte
uint8_t packet1[8] = {  0x01,       0x03,           0x00,0x00,           0x00,0x02,          0xC4,0x0B}; //V Phase A (a-n)
uint8_t packet2[8] = {  0x01,       0x03,           0x00,0x32,           0x00,0x02,          0x65,0xC4}; //Frequency
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  pinMode(dirpin, OUTPUT);
  // configure serial to connect with modbus device
  // and set <m_rtu> serial to let <m_rtu> use that serial
  // RXmax pin =23 -> RO pin :23,  TXmax pin =19 -> DI pin :19               
  Serial1.begin(9600,SERIAL_8N1,23,19);
  Serial.println("BEGIN");
  Serial.println();
}
void loop() {
digitalWrite(dirpin, HIGH); //set Pin Direction control to "HIGH" of send packet
delay(10);
Serial1.write(packet1, 8);//write packet
//https://www.arduino.cc/reference/en/language/functions/communication/serial/write/
delay(9);
digitalWrite(dirpin, LOW); //set Pin Direction control to "LOW" of receive packet
Data_recv(slave_addr);//receive packet
Serial.print("response packet1 = "); //print byte of the receive packet
for(int i=0; i<indexMax; i++) {
    Serial.print(buf[i],HEX);
    Serial.print(" ");
  }
Serial.println();
Serial.print("error (time out = 1)  = ");
Serial.println(m_error);
delay(15000);

digitalWrite(dirpin, HIGH); //set Pin Direction control to "HIGH" of send packet
delay(10);
Serial1.write(packet2, 8);//write packet
delay(9);
digitalWrite(dirpin, LOW); //set Pin Direction control to "LOW" of receive packet
Data_recv(slave_addr);//receive packet
Serial.print("response packet2 = "); //print byte of the receive packet
for(int i=0; i<indexMax; i++) {
    Serial.print(buf[i],HEX);
    Serial.print(" ");
  }
Serial.println();
Serial.print("error (time out = 1)  = ");
Serial.println(m_error);
delay(15000);
}


void Data_recv(uint8_t SS) {
  unsigned long prev_t = millis();  //resopnse check, time is over 20 sec set error code = 1 (Timeout Error code) 
  while(Serial1.available() == 0) {
    if(millis() - prev_t > 20000) { ///20000
      m_error = 1;      // TIMEOUT
      return;
    }
  }

  indexMax = 0;
   m_error = 0;
  prev_t = millis();                 // This section for keep data. Time between byte data are not over 500 ms. The data store in array buf[]. 
  while(Serial1.available() > 0) {   // If time between byte data  over 500 ms not accept.
    buf[indexMax++] = Serial1.read();
   // indexMax = indexMax + 1;
    while(Serial1.available() == 0) {
      if(millis()-prev_t > 500) {
        break;        // no more data
      }
    }
  }
}