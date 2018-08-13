#include <ESP8266WiFi.h>

#define WIFI_SSID       "xxxx"
#define WIFI_PASSWORD   "xxxxx"

void setup() {
  Serial.begin(9600);
  Serial.println(WiFi.localIP());
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  // Do something
}

void loop() {
  // Do something
}

