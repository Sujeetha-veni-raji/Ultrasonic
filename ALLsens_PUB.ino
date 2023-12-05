#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

//Initialize the LCD using the address of the I2C module
LiquidCrystal_I2C lcd(0x27, 16, 4);

#define DHT_SENSOR_PIN  14 // ESP8266 pin GPIO5 connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11
#define IR_SENSOR_PIN 2  // GPIO pin connected to the IR sensor
#define LDR_SENSOR_PIN 16  // GPIO pin connected to the LDR sensor

#define SMOKE_SENSOR_PIN A0  // GPIO4 (D2) pin connected to the smoke sensor

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

const char* ssid = "Airel_9842878776"; // Your WiFi SSID
const char* password = "air88581"; // Your WiFi password
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttClientID = "259a800e-1ac5-4109-aa6e-3714cbeefa14";
const char* mqttTopic = "Sensor";

String obstacleState = "no obstacle";
int ldrValue = 0; // Variable to store LDR reading
int smokeValue = 0; // Variable to store smoke sensor reading
WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqttClientID)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight

  dht_sensor.begin();

  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(LDR_SENSOR_PIN, INPUT);

  setupWiFi();
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  float humi =dht_sensor.readHumidity();
  float tempC =dht_sensor.readTemperature();



  
  
  int irSensorValue = digitalRead(IR_SENSOR_PIN);
  ldrValue = digitalRead(LDR_SENSOR_PIN);
  smokeValue = analogRead(SMOKE_SENSOR_PIN);
  Serial.println(irSensorValue); 


  

  if (irSensorValue == LOW){
    Serial.println("The obstacle is present");
    obstacleState="obstacle";
  } else{
    Serial.println("The obstacle is NOT present");
    obstacleState="no obstacle";}


  if (ldrValue == HIGH)
    Serial.println("ldr:It is dark");
  else
    Serial.println("ldr:It is light");
  // Print sensor data to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.print("°C, Humidity: ");
  Serial.print(humi);
  Serial.print("%, IR Sensor: ");
  Serial.print(obstacleState);
  Serial.print(", LDR Value: ");
  Serial.println(ldrValue);
  Serial.print(", Smoke Value: ");
  Serial.println(smokeValue);
  

  lcd.clear();              // clear display
  lcd.setCursor(0, 0);      // move cursor to (0, 0)
  lcd.print(tempC);       // print message at (0, 0)
  lcd.setCursor(2, 1);      // move cursor to (2, 1)
  lcd.print(humi); // print message at (2, 1)
  lcd.setCursor(3, 2);          // move cursor to (3, 0)
  lcd.print(obstacleState);        // print message at (3, 0)
  lcd.setCursor(4, 3);          // move cursor to (0, 1)
  lcd.print(ldrValue); // print message at (0, 1)
  lcd.setCursor(10,3);
  lcd.print(smokeValue);
  delay(2000);
  if (!client.connected()) {
    reconnect();
  }
  
  


  // Publish DHT, IR, and LDR sensor data to MQTT
  String data = String(tempC) + ","+ String(humi) + "," + String(obstacleState) + "," + String(ldrValue)+ "," + String(smokeValue);
  client.publish(mqttTopic, data.c_str());

  Serial.println("Published to MQTT: " + data);

  delay(2000);
}
