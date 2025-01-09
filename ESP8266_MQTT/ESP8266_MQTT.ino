
// Load libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// Uncomment one of the lines bellow for whatever DHT sensor type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321


// Replace with your network credentials
const char* ssid     = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "REPLACE_WITH_YOUR_MQTT_BROKER";


//Publish MQTT Topics
#define MQTT_PUB_TEMP "room/temperature" //change to reading 1 topic
#define MQTT_PUB_HUM "room/humidity" //change to reading 2 topic

//Subscribe MQTT Topics
#define OUTPUT_TOPIC "esp8266/output/test" //change to output 1 topic

//Timer to send data
#define PUSH_TIMER 1000

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);

// Variable to hold the temperature reading
String temperatureString = "";

// Set GPIOs for: output variable, status LED, PIR Motion Sensor, and LDR
const int DHTPin = 5;
const int lamp = 16;


// Store the current output state
String outputState = "off";


// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);



// Timers - Auxiliary variables
unsigned long now = millis();
unsigned long lastMeasure = 0;
boolean startTimer = false;
unsigned long currentTime = millis();
unsigned long previousTime = 0; 

// Don't change the function below. 
// This function connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT
  // If a message is received on the topic esp8266/output, you check if the message is either on or off. 
  // Turns the output according to the message received
  if(topic==OUTPUT_TOPIC){
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      digitalWrite(lamp, HIGH);
      Serial.print("on");
    }
    else if(messageTemp == "off"){
      digitalWrite(lamp, LOW);
      Serial.print("off");
    }
  }
  Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more outputs)
      client.subscribe(OUTPUT_TOPIC);  
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Checks if motion was detected and the sensors are armed. Then, starts a timer.
ICACHE_RAM_ATTR void detectsMovement() {
  Serial.println("MOTION DETECTED!");
  client.publish("esp8266/motion", "MOTION DETECTED!");
  previousTime = millis();
  startTimer = true;
}

void setup() {

  dht.begin();

  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // Initialize the output variable and the LED as OUTPUTs
  pinMode(lamp, OUTPUT); 
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Timer variable with current time
  now = millis();

  // Publishes new temperature and LDR readings every 30 seconds
  if (now - lastMeasure > PUSH_TIMER) {
    lastMeasure = now;
        // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float humidity = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float temperatureC = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float temperatureF = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperatureC) || isnan(temperatureF)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Publishes Temperature and Humidity values
    client.publish(MQTT_PUB_TEMP, String(temperatureC).c_str());
    client.publish(MQTT_PUB_HUM, String(humidity).c_str());
    //Uncomment to publish temperature in F degrees
    //client.publish("room/temperature", String(temperatureF).c_str());
    
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" ºC");
    Serial.print(temperatureF);
    Serial.println(" ºF");

}
}
