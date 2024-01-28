#include <WiFi.h>
#include <PubSubClient.h>
//#include "DHT.h"

const char* ssid = "cs-mtg-room";
const char* password = "bilik703";
const char* MQTT_SERVER = "34.132.181.18"; // your VM instance public IP address
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "water-level"; // MQTT topic

/* Water level thresholds */
int lowerThreshold = 4000;
int upperThreshold = 2500;

// Sensor pins
#define sensorPower 21
#define sensorPin A0
int val = 0;

// Declare pins to which LEDs are connected
int redLED = 48;
int yellowLED = 38;
int greenLED = 39;



// DHT configuration


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  // Print connection status
  Serial.println("\nConnected to the WiFi network");
  Serial.print("Maker Feather AIoT S3 IP: ");
  Serial.println(WiFi.localIP());

  // Sensor setup
  pinMode(sensorPower, OUTPUT);
  digitalWrite(sensorPower, LOW);

  // LED setup
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  digitalWrite(redLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(greenLED, LOW);

  // MQTT setup
  //setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  //dht.begin();
}

// MQTT reconnect
void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT server");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}


void loop() {
  // MQTT communication for water level sensing
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(5000);

  // Water level monitoring
  int level = readSensor();

  if (level == 0) {
    Serial.println("Water Level: Empty");
    digitalWrite(redLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(greenLED, LOW);
  } else if (level < upperThreshold) {
    Serial.println("Water Level: High");
    digitalWrite(redLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(greenLED, LOW);
  } else if (level < lowerThreshold && level >= upperThreshold) {
    Serial.println("Water Level: Low");
    digitalWrite(redLED, LOW);
    digitalWrite(yellowLED, HIGH);
    digitalWrite(greenLED, LOW);
  } else if (level > lowerThreshold) {
    Serial.println("Water Level: Empty");
    digitalWrite(redLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(greenLED, HIGH);
  }

   // adjust the delay according to your requirements
  int waterLevel = analogRead(sensorPin);
  Serial.print("Water Level: ");
  Serial.println(waterLevel);

  delay(1000);

  char payload[10];
  sprintf(payload, "%d", waterLevel);
  client.publish(MQTT_TOPIC, payload);
}

// Water level sensor reading
int readSensor() {
  digitalWrite(sensorPower, HIGH);
  delay(10);
  val = analogRead(sensorPin);
  digitalWrite(sensorPower, LOW);
  return val;
}



