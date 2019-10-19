#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)

#define WIFI_SSID    "6021607"
#define WIFI_PASS    "17401449"

Adafruit_BME280 bme;
bool status;

void setup() {
  Wire.begin();
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
  Serial.println(F("BME280 CHECK"));
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  oled.init();
  oled.clearDisplay();
  oled.setTextXY(3, 0);
  oled.putString("BME280 Start...");
  delay(1000);
  oled.clearDisplay();
}
void loop() {
  Serial.print("Temperature = ");
  float Temperature = bme.readTemperature();
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  oled.setTextXY(0, 0);
  oled.putString("T:" + String(Temperature) + " *c");

  Serial.print("Pressure = ");
  float Pressure = bme.readPressure() / 100.0F;
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");
  oled.setTextXY(1, 0);
  oled.putString("P:" + String(Pressure) + " hPa");

  Serial.print("Approx. Altitude = ");
  float Approx = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
  oled.setTextXY(2, 0);
  oled.putString("A:" + String(Approx) + " m");

  Serial.print("Humidity = ");
  float Humidity = bme.readHumidity();
  Serial.print(bme.readHumidity());
  Serial.println(" %");
  oled.setTextXY(3, 0);
  oled.putString("H:" + String(Humidity) + " %");
  // call report
  Sendvalue(Temperature, Pressure, Approx, Humidity);
  delay(5000);
}
void Sendvalue(float temperature, float pressure, float approx, float humidity) {

  HTTPClient http;
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  JSONencoder["temperature"] = temperature;
  JSONencoder["pressure"] = pressure;
  JSONencoder["approx"] = approx;
  JSONencoder["humidity"] = humidity;
  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  //Serial.println(JSONmessageBuffer);

  Serial.println("Authenticating Device...");
  http.begin("http://192.168.0.5/Dashboard-BME280S/input.php");
  http.addHeader("User-Agent", "ESP8266HTTPClient");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("Connection", "close");

  int httpCode = http.POST(JSONmessageBuffer);
  String payload = http.getString();
  Serial.println("HTTPS CODE :" + String(httpCode));
  Serial.println("Payload :" + String(payload));
  http.end();
}
