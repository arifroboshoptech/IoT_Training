#include <WiFi.h>
#include <MQTT.h>
#include "DHT.h"
 
//NETWORK SETTING
#define WIFI_SSID       "************"
#define WIFI_PASSWORD   "************"

//MQTT SETTING
#define MQTT_server       "broker.hivemq.com" r
#define MQTT_topic        "MyReskill_IoT/YourDeploymentLocation/Nodes1"
#define MQTT_publish      "/DHT11"
#define MQTT_subscribe    "/status"

 
WiFiClient net;
MQTTClient client;

#define DHTPIN 2 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "try", "try")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
  client.subscribe(String(MQTT_topic)+ String(MQTT_publish));
}
 
void setup() { 
   Serial.begin(115200);
   dht.begin();
   WiFi.begin(ssid, pass);
   client.begin(MQTT_server, net);
   client.onMessage(messageReceived);
   connect();
}
void loop() {
  client.loop();
  delay(10);  

  if (!client.connected()) {
    connect();
  }

 
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C "));
    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.println(F("% "));

    String datainJSON = "{";
    datainJSON += "\"temperature\":" + String(temperature) + ",";
    datainJSON += "\"humidity\":" + String(humidity);
    datainJSON += "}";

    Serial.println("Data to Publish: " + datainJSON);
    Serial.println("Length of Data: " + String(datainJSON.length());
    Serial.println("Publish to: " + String(MQTT_topic) + String(MQTT_publish)); 
    
    client.publish(String(MQTT_topic)+ String(MQTT_publish), datainJSON);
  }
}
 
