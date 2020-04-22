#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>
#define DoorGpioPin D1
#define ledGpioPin D2
#define closed 0
#define opened 1
#define WIFI_SSID   "6021607"
#define WIFI_PASS   "17401449"

int doorState = 0;
int ledState = 0;

void setup() {
  Serial.begin(115200);
  pinMode(DoorGpioPin, INPUT);
  pinMode(ledGpioPin, OUTPUT);
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
  if (digitalRead(DoorGpioPin) == HIGH && doorState == opened) {
    LINE.notify("ปิด");
    Serial.println("Close");
    doorState = closed;
    ledState = 1; //on
    digitalWrite(ledGpioPin, ledState);
  }
  if (digitalRead(DoorGpioPin) == LOW && doorState == closed) {
    LINE.notify("เปิด");
    Serial.println("Open");
    doorState = opened;
    ledState = 0; //off
    digitalWrite(ledGpioPin, ledState);
  }
}
