// Read DHT11 sensor and send serially to PC

#include <DHT.h>         // Include Adafruit DHT11 Sensors Library

#define DHTPIN 23          // DHT11 Output Pin connection
#define DHTTYPE DHT11     // DHT Type is DHT11

DHT dht(DHTPIN, DHTTYPE);   // Initialize DHT sensor

void setup () {
  dht.begin();
  Serial.begin(115200);         // To see data on serial monitor
}

void loop (){

    float H = dht.readHumidity();     //Read Humidity
    float T = dht.readTemperature();    // Read temperature as Celsius

    // Check if any reads failed and if exit
    if (isnan(H) || isnan(T)){
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
 
    // Combine Humidity and Temperature into single string
    String dhtData = String(T) + "," + String(H);
    Serial.println(dhtData);
    delay(2000);   // Wait two seconds between measurements
}
