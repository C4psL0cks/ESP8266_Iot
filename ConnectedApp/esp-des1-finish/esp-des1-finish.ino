#include <ESP8266WiFi.h>
#include <OneWire.h>
OneWire  ds(4);
#define WIFI_SSID       "6021607"
#define WIFI_PASSWORD   "17401449"
String data;

void setup() {
  Serial.begin(115200);
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
}
void loop() {
  if (Serial.available()) {
    char check = Serial.read();
    if (check == 'a') {
      Serial.println("up");
      getDeviceAddress();
      data = "";
    }
    else if (check == 'b') {
      Serial.println("reset");
      data = "";
    }
  }

}
void getDeviceAddress(void) {
  byte i;
  byte addr[8];
  Serial.println("Getting the address...\n\r");
  while (ds.search(addr)) {
    Serial.print("The address is:\t");
    for ( i = 0; i < 8; i++) {
      //Serial.print(addr[i]);
      data += addr[i];
    }
    if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n");
      return;
    }
  }
  Serial.println();
  Serial.println("DATA :" + data);
  ds.reset_search();
  return;
}
