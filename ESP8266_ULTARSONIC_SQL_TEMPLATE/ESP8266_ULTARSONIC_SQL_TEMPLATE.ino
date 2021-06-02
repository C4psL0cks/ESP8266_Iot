#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define WIFI_SSID   "dlink-ABD0"
#define WIFI_PASS   "yyafr68490"

HTTPClient http;
long duration,distance,cm;
#define TRIGGER_PIN  5
#define ECHO_PIN     4

void setup() {
  Serial.begin(115200);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
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
  digitalWrite(TRIGGER_PIN, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration/2) / 29.1;
  Serial.print(distance);
  Serial.println(" cm");
  report(distance);
  delay(1000);
}

void report(double cm) {

  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["cm"] = cm;
  JSONencoder["id_iot"] = "001";
  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);
  Serial.println("Authenticating Device...");

  if (WiFi.status() == WL_CONNECTED) {

    http.begin("http://35.187.225.157/api-new/sensorinput.php");
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
  delay(1000);
}
