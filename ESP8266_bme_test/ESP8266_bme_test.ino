#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)

const char *WIFI_SSID = "6021607";
const char *WIFI_PASS = "17401449";

Adafruit_BME280 bme;
unsigned long delayTime;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {

    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.println(F("BME280 test"));
  bool status;
  status = bme.begin(0x76);
  if (!status)
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1)
      ;
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
  delay(10000);
}
void report(double temperature, double pressure, double approx, double humidity)
{

  StaticJsonBuffer<300> JSONbuffer;
  JsonObject &JSONencoder = JSONbuffer.createObject();

  JSONencoder["temperature"] = temperature;
  JSONencoder["pressure"] = pressure;
  JSONencoder["approx"] = approx;
  JSONencoder["humidity"] = humidity;

  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  //Serial.println(JSONmessageBuffer);
  Serial.println("Authenticating Device...");
  HTTPClient http;
  http.begin("http://1b1c47a5.ngrok.io/code/dbtestbme/inputjson.php");
  http.header("User-Agent: ESP8266:Node1");
  http.header("Connection: close");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  int httpCode = http.POST(JSONmessageBuffer);
  String payload = http.getString();
  Serial.println("HTTPS CODE :" + String(httpCode));
  Serial.println("Payload :" + String(payload));
  http.end();
}
