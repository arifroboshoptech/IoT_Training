/*************************************************************

  DHT + Traffic Light Module using Blynk

 *************************************************************/

#define BLYNK_TEMPLATE_ID           "TMPxxxxxx"
#define BLYNK_TEMPLATE_NAME         "Device"
#define BLYNK_AUTH_TOKEN            "YourAuthToken"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// WiFi credentials
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";

// DHT Sensor Config
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

BlynkTimer timer;

// Traffic Light GPIO Pins
#define RED_PIN    16  // D0
#define YELLOW_PIN 5   // D1
#define GREEN_PIN  4   // D2

// ======= DHT Sensor Reading =======
void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V5, h);  // Humidity to V5
  Blynk.virtualWrite(V6, t);  // Temperature to V6
}

// ======= Blynk Virtual Pin Handlers =======
BLYNK_WRITE(V1) {  // Red Light Control
  int redState = param.asInt();
  digitalWrite(RED_PIN, redState);
  Serial.print("Red Light: ");
  Serial.println(redState ? "ON" : "OFF");
}

BLYNK_WRITE(V2) {  // Yellow Light Control
  int yellowState = param.asInt();
  digitalWrite(YELLOW_PIN, yellowState);
  Serial.print("Yellow Light: ");
  Serial.println(yellowState ? "ON" : "OFF");
}

BLYNK_WRITE(V3) {  // Green Light Control
  int greenState = param.asInt();
  digitalWrite(GREEN_PIN, greenState);
  Serial.print("Green Light: ");
  Serial.println(greenState ? "ON" : "OFF");
}

void setup() {
  Serial.begin(115200);

  // Initialize Traffic Light Pins
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  // Turn off all LEDs initially
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);

  // Start DHT Sensor
  dht.begin();

  // Connect to Blynk Cloud
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Timer for DHT sensor reading
  timer.setInterval(1000L, sendSensor);
}

void loop() {
  Blynk.run();
  timer.run();
}
