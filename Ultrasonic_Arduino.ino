#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>





const int trigPin = D0;    // Trig pin of the ultrasonic sensor
const int echoPin = D7;   // Echo pin of the ultrasonic sensor




const char* ssid = "Airel_9842878776";
const char* password = "air88581";
const char* mqttServer = "broker.emqx.io"; // MQTT broker address
const int mqttPort = 1883; // MQTT broker port
const char* mqttUser = ""; // MQTT username (if required)
const char* mqttPassword = ""; // MQTT password (if required)

//DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

    Serial.begin(115200);
    WiFi.begin(ssid, password);

    pinMode(trigPin, OUTPUT); // Set the trigPin as an OUTPUT
    pinMode(echoPin, INPUT);  // Set the echoPin as an INPUT
 

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
    client.setServer(mqttServer, mqttPort);
   
}

void reconnect() {
    while (!client.connected()) {
        Serial.println("Attempting MQTT connection...");
        if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
            Serial.println("Connected to MQTT broker");
        }
        else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void loop() {


  // Variables to store the distance and duration
  long duration;
  int distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the echo pulse
  duration = pulseIn(echoPin, HIGH);

 
  // Calculate the distance in centimeters
  distance = duration * 0.034 / 2;



  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");



    if (!client.connected()) {
        reconnect();
    }

    String data = String(distance);
    String topic = "Sensors19"; // MQTT topic to publish to
    client.publish(topic.c_str(), data.c_str());

    Serial.println("Published to MQTT: " + data);
    delay(1000);
}
