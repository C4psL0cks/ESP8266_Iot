#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define WIFI_SSID   "6021607"
#define WIFI_PASS   "17401449"

HTTPClient http;

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
}

void loop() {

  float value1 = 3;
  float value2 = 4;
  float value3 = 5;
  report(value1 , value2, value3);
}

void report(double value1, double value2, double value3) {

  StaticJsonBuffer<300> JSONbuffer;   
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["value1"] = value1;
  JSONencoder["value2"] = value2;
  JSONencoder["value3"] = value3;

  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);
  Serial.println("Authenticating Device...");

  if (WiFi.status() == WL_CONNECTED) {

    http.begin("http://xxx");                                                                                           
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
    else{
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(HTTP_CODE).c_str());
    }
    http.end();
    Serial.println("Reported!");
  }
  delay(1000);                                                                       
}
