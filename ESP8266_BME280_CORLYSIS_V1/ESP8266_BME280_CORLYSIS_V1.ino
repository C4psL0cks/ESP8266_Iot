//https://www.hackster.io/michal-kren/spacex-remote-monitoring-system-e57852
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
// Corlysis Setting - click to the database to get those info
const char* db_name = "sensor_db";
const char* db_password = "1f7bad37f797bb8119fd164e66a1a936";
const unsigned long delayTimeMs = 20000;

Adafruit_BME280 bme;
HTTPClient http;
bool status;

void setup() {
  Serial.begin(115200);
  Serial.println("NodeMCU + DHT11 + Corlysis");
  //Wi-Fi connection
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
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}


void loop() {

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
  sendDataToCorlysis(Temperature, Humidity);
  delay(delayTimeMs);
}

void sendDataToCorlysis(float temperature, float humidity) {
  static long counter = 0;

  char payloadStr[150];
  sprintf(payloadStr, "dht11_data temperature=%d.%02d,humidity=%d.%02d", (int)temperature, (int)abs(temperature * 100) % 100, (int)humidity, (int)abs(humidity * 100) % 100);
  Serial.println(payloadStr);

  char corlysisUrl[200];
  sprintf(corlysisUrl, "http://corlysis.com:8087/write?db=%s&u=token&p=%s", db_name, db_password);
  http.begin(corlysisUrl);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(payloadStr);
  Serial.print("http result:");
  Serial.println(httpCode);
  http.writeToStream(&Serial);
  http.end();

  if (httpCode == 204) {
    counter = 0;
    Serial.println("Data successfully sent.");
  } else {
    if (counter > 10 && httpCode == -1) {
      Serial.println("WiFi: still not connected -> going into deep sleep for 10 seconds.");
      ESP.deepSleep(10e6);
    }
    counter++;
    Serial.println("Data were not sent. Check network connection.");
  }
  Serial.println("");
}
