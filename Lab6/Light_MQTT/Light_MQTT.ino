#include "mqtt_secrets.h"
#include <HTTPClient.h>
#include <PubSubClient.h>

// replace with your wifi ssid and wpa2 key
const char* ssid = "narest_2.4G";
const char* pass = "narest6552";

const char* server = "mqtt3.thingspeak.com";
const char* channelID = "2456023"; 
const char* mqttUserName = SECRET_MQTT_USERNAME;
const char* mqttPass = SECRET_MQTT_PASSWORD;
const char* clientID = SECRET_MQTT_CLIENT_ID;
#define LED_IOT 12
#define LDR_Pin 36
WiFiClient client;
PubSubClient mqtt(client);
void setup() {
  pinMode(LED_IOT, OUTPUT);
  pinMode(LDR_Pin, INPUT);
  Serial.begin(115200);
  mqtt.setServer(server, 1883);
}
void loop() {
  unsigned static int half_second_count = 0;  //

  // Check if WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    //Connect to Router
    Serial.print("Connecting to :");
    Serial.print(ssid);
    Serial.print(" : ");
    //Connect ESP32 to home network
    WiFi.begin(ssid, pass);
    //Wait until Connection is complete
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nConnected.");
  }
  //Check if MQTT Server is connected
  if (!mqtt.connected()) {
    Serial.println("Connecting to MQTT Broker.");
    mqtt.connect(clientID, mqttUserName, mqttPass);
    while (!mqtt.connected()) {
      delay(500);
      Serial.print(".");
    }
  }
  //Post http get to ThingSpeak Every 15 seconds
  if (half_second_count >= 30) {
    //Reset Timer
    half_second_count = 0;
    uint16_t ligth = analogRead(LDR_Pin);
    String dataString = "&field1=" + String(ligth);
    String topicString = "channels/" + String(channelID) + "/publish";
    mqtt.publish(topicString.c_str(), dataString.c_str());
    Serial.println("Ligth Value = " + String(ligth));
    digitalWrite(LED_IOT, !digitalRead(LED_IOT));
  }
  //Update Half seconds every 500 ms
  delay(500);
  half_second_count++;
}
