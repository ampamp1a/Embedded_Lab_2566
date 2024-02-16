const int slave_addr = 1;   // Slave Address of Modbus Device
const int dirpin = 18;      // Direction Control Pin 
uint8_t buf[128];
int8_t indexMax = 0;
int8_t m_error = 0;
int8_t ready = 0;
unsigned long prev_t;
uint8_t packet[6]; // for recieve from user and sent to CRC Algorithm
uint8_t packet1[8]; // for sent packet to power meter
String Ex_String_Read; // keep data from serial monitor
unsigned long address,dataAdd1,dataAdd2,dataLen1,dataLen2,crc1,crc2; // data in packet
uint16_t crc;
int Result; // Result to show for user
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

void get_value(String value) {

  if (sscanf(value.c_str(), "address: %x data address: %x %x data length: %x %x",&address,&dataAdd1,&dataAdd2,&dataLen1,&dataLen2) == 5) {
    //seperate to keep data for packet

    // show value each data for keep packet
    Serial.print("Address: ");
    Serial.println(address,HEX);
    Serial.print("Data address: ");
    Serial.print(dataAdd1,HEX);
    Serial.println(dataAdd2,HEX);
    Serial.print("Data length: ");
    Serial.print(dataLen1,HEX);
    Serial.println(dataLen2,HEX);  

    // keep data in packet to sent for calculate crc
        packet[0] = address & 0xFF; //address
        packet[1] = 0x03; // Function code
        packet[2] = dataAdd1 & 0xFF; //dataAdd1
        packet[3] = dataAdd2 & 0xFF; //dataAdd2
        packet[4] = dataLen1 & 0xFF; //dataLen1
        packet[5] = dataLen2 & 0xFF; //dataLen2
        
    crc = MODBUS_CRC16_v1(packet,6); // sent packet for calculate crc 
    crc1 = crc & 0xFF;  // LSB
    crc2 = (crc >> 8) & 0xFF; // MSB
    // keep data in packet to sent for Power meter
        packet1[0] = packet[0]; //address
        packet1[1] = 0x03; // Function code
        packet1[2] = packet[2]; //dataAdd1
        packet1[3] = packet[3]; //dataAdd2
        packet1[4] = packet[4]; //dataLen1
        packet1[5] = packet[5]; //dataLen2
        packet1[6] = crc1;
        packet1[7] = crc2;
    ready = 1;

  } else {
    Serial.println("Invalid Format");
  }
}

static uint16_t MODBUS_CRC16_v1( const unsigned char *buf, unsigned int len )
{
	uint16_t crc = 0xFFFF;
	unsigned int i = 0;
	char bit = 0;

	for( i = 0; i < len; i++ )
	{
		crc ^= buf[i];

		for( bit = 0; bit < 8; bit++ )
		{
			if( crc & 0x0001 )
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
			{
				crc >>= 1;
			}
		}
	}

	return crc;
}

void loop() {

while(Serial.available()) {  // วนรับค่าหากด้วย while loop ถ้าหาก Serial.available() (ถ้า Serial ถูกใช้งาน)
    Ex_String_Read = Serial.readString();  // ใช้คำสั้ง Serial.readString() สำหรับเก็บค่า String ไว้ในตัวแปร Ex_String_Read
    Serial.print("Ex_String_Read = ");
    Serial.println(Ex_String_Read);  // แสดงผลออกทาง Serial
    get_value(Ex_String_Read); // for call function to prepare packet to sent power meter

     if (ready = 1){
        for (int i = 0; i < sizeof(packet1) / sizeof(packet1[0]); i++) {
        Serial.print("packet ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(packet1[i], HEX); // Print in hexadecimal format
      }

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
        for(int i=3; i<7; i++) {
            Result = (Result << 8) | buf[i];
        }
        
        Serial.println(Result*0.001);
        Serial.print("error (time out = 1)  = ");
        Serial.println(m_error);
        delay(15000);
        ready = 0;
    }
}

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
//address: 0x01 data address: 0x00 0x32 data length: 0x00 0x02 //for test frequency // 50 Hz
