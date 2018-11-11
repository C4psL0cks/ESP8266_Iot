#include <ESP8266WiFi.h>
void setup() {
  Serial.begin(115200);
  Serial.printf(" ESP8266 Chip id = %08X\n", ESP.getChipId());
}
void loop() {

}
