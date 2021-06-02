#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>

#define WIFI_SSID   "dlink-ABD0"
#define WIFI_PASS   "yyafr68490"
#define LINE_TOKEN  "-----------"

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
  LINE.setToken(LINE_TOKEN);
  delay(500);
  LINE.notify("รอการเชื่อมต่อเวลาสักครู่...สำเร็จแล้ว");

}

void loop() {
  // put your main code here, to run repeatedly:

}
