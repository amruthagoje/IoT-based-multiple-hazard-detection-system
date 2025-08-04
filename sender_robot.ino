#include <DHT.h>
#include <LoRa.h>
#include <SPI.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include <BluetoothSerial.h>
// WiFi credentials
const char *ssid = "ESP_IoT_Lab";
const char *password = "ESP32_IoT";
// ThingSpeak channel settings
const unsigned long channelID = 2517267;
const char *apiKey = "2ODFRK0BDA672KCS";
// Define pin numbers for sensors
#define DHTPIN 32 // Digital pin connected to the DHT sensor
#define PIR_SENSOR_PIN 33 // Pin number for PIR sensor
#define IR_SENSOR_PIN 35 // Pin number for IR sensor
#define MQ_PIN 34 // Pin number for MQ135 sensor
#define SENSOR_PIN 25 // Pin number for proximity sensor
#define motor1Input1A 4 // Motor control pins
#define motor1Input1B 0
#define motor2Input2A 26
#define motor2Input2B 27
WiFiClient client;
BluetoothSerial serialBT;
// Define sensor types
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);
#define ss 5
#define rst 14
#define dio0 2
void setup() {
 Serial.begin(115200);
 while (!Serial)
 ;
 Serial.println("Sensors Test");
 // Initialize sensors
 dht.begin();
 pinMode(PIR_SENSOR_PIN, INPUT);
 pinMode(IR_SENSOR_PIN, INPUT);
 pinMode(MQ_PIN, INPUT);
 pinMode(SENSOR_PIN, INPUT);
 // Set motor control pins as outputs
 pinMode(motor1Input1A, OUTPUT);
 pinMode(motor1Input1B, OUTPUT);
 pinMode(motor2Input2A, OUTPUT);
 pinMode(motor2Input2B, OUTPUT);
 // Activate motor 1
 digitalWrite(motor1Input1A, HIGH);
 digitalWrite(motor1Input1B, LOW);
 // Activate motor 2
 digitalWrite(motor2Input2A, HIGH);
 digitalWrite(motor2Input2B, LOW);
 LoRa.setPins(ss, rst, dio0); // Setup LoRa transceiver module
 while (!LoRa.begin(433E6)) { // 433E6 - Asia, 866E6 - Europe, 915E6 -
North America
 Serial.println(".");
 delay(500);
 }
 LoRa.setSyncWord(0xA5);
 Serial.println("LoRa Initializing OK!");
 // Connect to WiFi
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.println("Connecting to WiFi...");
 }
Serial.println("Connected to WiFi");
 // Initialize ThingSpeak
 ThingSpeak.begin(client);
 // Start Bluetooth serial communication
 serialBT.begin("Tarzan");
}
void loop() {
 // DHT Sensor reading
 float humidity = dht.readHumidity();
 float temperature = dht.readTemperature();
 String message = "Temperature: " + String(temperature) + " °C, Humidity: " +
String(humidity) + " %\t";
 // PIR Sensor reading
 int pirSensorValue = digitalRead(PIR_SENSOR_PIN);
 message += "PIR Sensor Value: " + String(pirSensorValue) + ", ";
 // IR Sensor reading
 int irSensorValue = digitalRead(IR_SENSOR_PIN);
 message += "IR Sensor Value: " + String(irSensorValue) + ", ";
 // MQ135 Sensor reading
 int mqsensorValue = analogRead(MQ_PIN);
 float voltage = mqsensorValue * (3.3 / 4095.0);
 float ppm = map(voltage, 0.1, 3.0, 0, 500);
 message += "Gas Concentration (PPM): " + String(ppm) + ", ";
 // Metal Sensor reading
 int metalsensorValue = digitalRead(SENSOR_PIN);
 message += "Metal Sensor Value: " + String(metalsensorValue);
 Serial.println("Sending packet: " + message);
 // Send sensor data to ThingSpeak
 ThingSpeak.writeField(channelID, 1, temperature, apiKey);
 ThingSpeak.writeField(channelID, 2, humidity, apiKey);
 ThingSpeak.writeField(channelID, 3, pirSensorValue, apiKey);
 ThingSpeak.writeField(channelID, 4, irSensorValue, apiKey);
 ThingSpeak.writeField(channelID, 5, ppm, apiKey);
 ThingSpeak.writeField(channelID, 6, metalsensorValue, apiKey);
 // LoRa communication
 LoRa.beginPacket(); // Send LoRa packet to receiver
 LoRa.print(message);
 LoRa.endPacket();
 // Bluetooth motor control
 if (serialBT.available()) {
 char cmd = serialBT.read();
 switch (cmd) {
 case 'F': // Forward
 digitalWrite(motor1Input1A, HIGH);
 digitalWrite(motor1Input1B, LOW);
 digitalWrite(motor2Input2A, HIGH);
 digitalWrite(motor2Input2B, LOW);
 break;
 case 'B': // Backward
 digitalWrite(motor1Input1A, LOW);
 digitalWrite(motor1Input1B, HIGH);
 digitalWrite(motor2Input2A, LOW);
 digitalWrite(motor2Input2B, HIGH);
 break;
 case 'L': // Left
 digitalWrite(motor1Input1A, LOW);
 digitalWrite(motor1Input1B, HIGH);
 digitalWrite(motor2Input2A, HIGH);
 digitalWrite(motor2Input2B, LOW);
 break;
 case 'R': // Right
 digitalWrite(motor1Input1A, HIGH);
 digitalWrite(motor1Input1B, LOW);
 digitalWrite(motor2Input2A, LOW);
 digitalWrite(motor2Input2B, HIGH);
 break;
 case 'S': // Stop
 digitalWrite(motor1Input1A, LOW);
 digitalWrite(motor1Input1B, LOW);
 digitalWrite(motor2Input2A, LOW);
 digitalWrite(motor2Input2B, LOW);
 break;
 default:
 break;
 }
 }
 delay(15000); // Send data every 15 seconds (ThingSpeak limit)
}