#include <Wire.h>
#include <ACROBOTIC_SSD1306.h> //i2 spi
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // i2 spi

bool status;
float Temperature;
float Pressure;
float Altitude;
float Humidity;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println(F("BME280 Start......"));
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  oled.init();                      // Initialze SSD1306 OLED display
  oled.clearDisplay();              // Clear screen
  oled.setTextXY(3, 0);             // Set cursor position, start of line 0
  oled.putString("BME280 Start...");
  delay(1000);
  oled.clearDisplay();              // Clear screen
}

void loop() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  Temperature = bme.readTemperature();
  oled.setTextXY(0, 0);             // Set cursor position, start of line 0
  oled.putString("T:" + String(Temperature) + " *c");

  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");
  Pressure = bme.readPressure() / 100.0F;
  oled.setTextXY(1, 0);             // Set cursor position, start of line 1
  oled.putString("P:" + String(Pressure) + " hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
  Altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  oled.setTextXY(2, 0);             // Set cursor position, start of line 2
  oled.putString("A:" + String(Altitude) + " m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
  Humidity = bme.readHumidity();
  oled.setTextXY(3, 0);            // Set cursor position, line 2 10th character
  oled.putString("H:" + String(Humidity) + " %");

  Serial.println();
  delay(1000);
}
