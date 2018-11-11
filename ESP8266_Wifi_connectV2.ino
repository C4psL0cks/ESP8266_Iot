#include <ESP8266WiFi.h>
#define WIFI_SSID       "6021607"
#define WIFI_PASSWORD   "17401449"

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
