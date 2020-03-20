#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT22
#define WIFI_SSID   "6021607"
#define WIFI_PASS   "17401449"

DHT dht(DHTPIN, DHTTYPE);
HTTPClient http;

int timeSinceLastRead = 0;

void setup() {
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
  dht.begin();
}

void loop() {


  if (timeSinceLastRead > 2000) {

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }
    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");
    report(h, t, f, hic, hif); 
    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100;
}

void report(double humidity, double tempC, double tempF, double heatIndexC, double heatIndexF) {

  if (WiFi.status() == WL_CONNECTED) {

    StaticJsonBuffer<300> JSONbuffer;
    JsonObject& JSONencoder = JSONbuffer.createObject();

    JSONencoder["humidity"] =  humidity;
    JSONencoder["tempC"] = tempC;
    JSONencoder["tempF"] = tempF;
    JSONencoder["heatIndexC"] = heatIndexC;
    JSONencoder["heatIndexF"] = heatIndexF;

    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    Serial.println("Authenticating Device...");

    http.begin("http://3d2f9a7e.ngrok.io/code/node/inputjson.php");
    http.addHeader("User-Agent", "ESP8266HTTPClient");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Accept", "application/json");
    http.addHeader("Connection", "close");

    int HTTP_CODE = http.POST(JSONmessageBuffer);
    if (HTTP_CODE > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", HTTP_CODE);
      if (HTTP_CODE == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.print("HTTPS CODE :");
        Serial.println(HTTP_CODE);
        Serial.print("Payload :");
        Serial.println(payload);
      }
    }
    else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(HTTP_CODE).c_str());
    }
    http.end();
    Serial.println("Reported!");
  }
}
