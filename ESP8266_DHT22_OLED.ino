#include "DHT.h"                    //Include DHT library
#include <Wire.h>                   // Include library
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET -1
Adafruit_SSD1306 OLED(OLED_RESET);  // New object OLED

#define DHTTYPE DHT22
#define DHTPIN  D4
DHT dht (DHTPIN, DHTTYPE);
void setup()   {
  Serial.begin(115200);
  dht.begin();
  OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // initialize with the I2C addr 0x3C (for the 128x64)
}
void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");

  }
  OLED.clearDisplay();               //Clear display
  OLED.setTextColor(WHITE);          //Set text color
  OLED.setCursor(0, 0);              //Set display start position
  OLED.setTextSize(2);               //Set text size x2
  OLED.println("H: " + String(h, 0) + " %"); // Show result value
  Serial.println("H: " + String(h) + " %"); // Show result value
  OLED.setCursor(0, 30);            //Set display postion
  OLED.print("T: " + String(t, 1) + " ");
  Serial.println("T: " + String(t) + " ");
  OLED.write(247);
  OLED.print("C");
  OLED.display();                    //Enable display
  delay(5000);
  OLED.clearDisplay();               //Clear display
  

}


