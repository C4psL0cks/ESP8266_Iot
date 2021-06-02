#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)

#define WIFI_SSID   "dlink-ABD0"
#define WIFI_PASS   "yyafr68490"

Adafruit_BME280 bme;
bool status;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("Running!");
  Serial.println("-------------------------------------");
  // Connect to Wifi.
  Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("WiFi Connecting to %s\n", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.println(F("BME280 test"));
  status = bme.begin(0x76);
  if (!status){
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}
void loop()
{
  Serial.print("Temperature = ");
  float Temperature = bme.readTemperature();
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");
  float Pressure = bme.readPressure() / 100.0F;
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  float Approx = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  float Humidity = bme.readHumidity();
  Serial.print(bme.readHumidity());
  Serial.println(" %");
  Serial.println();
  report(Temperature, Pressure, Approx, Humidity);
  delay(1000);
}
void report(double temperature, double pressure, double approx, double humidity){

  StaticJsonBuffer<300> JSONbuffer;
  JsonObject &JSONencoder = JSONbuffer.createObject();

  JSONencoder["temperature"] = temperature;
  JSONencoder["pressure"] = pressure;
  JSONencoder["approx"] = approx;
  JSONencoder["humidity"] = humidity;

  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);
}

