#include <LoRa.h>
#include <U8g2lib.h>
#include <SPI.h>
#define ss 5
#define rst 14
#define dio0 2
#define SDA_PIN 21
#define SCL_PIN 22
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/
U8X8_PIN_NONE, SCL_PIN, SDA_PIN);
void setup() {
 Serial.begin(115200);
 while (!Serial);
 Serial.println("LoRa Receiver");
 LoRa.setPins(ss, rst, dio0); // Setup LoRa transceiver module
 while (!LoRa.begin(433E6)) { // 433E6 - Asia, 866E6 - Europe, 915E6 -
North America
 Serial.println(".");
 delay(500);
 }
 LoRa.setSyncWord(0xA5);
 Serial.println("LoRa Initializing OK!");
 // Initialize the OLED display
 u8g2.begin();
}
void loop() {
 int packetSize = LoRa.parsePacket(); // Try to parse packet
 if (packetSize) {
 Serial.print("Received packet: '");
 while (LoRa.available()) { // Read packet
 String LoRaData = LoRa.readString();
 Serial.print(LoRaData);
 // Clear the display buffer
 u8g2.clearBuffer();

 // Set font and position
 u8g2.setFont(u8g2_font_ncenB08_tr);
 u8g2.setCursor(0, 10);

 // Print the received LoRa data
 u8g2.print(LoRaData);
 u8g2.setCursor(0, 20);
 u8g2.print(humidity);
 u8g2.setCursor(0, 30);
 u8g2.print(Pir sensor value);
 u8g2.setCursor(0, 40);
 u8g2.print(Gas concentration);
 u8g2.setCursor(0, 50);
 u8g2.print(Metal detection):
 u8g2.setCursor(0, 60);
 u8g2.print(Lat-21.123570, Lng-79.049217);

 // Send the buffer to the display
 u8g2.sendBuffer();
 }
 Serial.print("' with RSSI "); // Print RSSI of packet
 Serial.println(LoRa.packetRssi());
 }
}