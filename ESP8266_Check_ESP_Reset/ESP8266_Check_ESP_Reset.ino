#include <Arduino.h>

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif

void setup() {
  rst_info *rsti;

  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Booted ");

  rsti = ESP.getResetInfoPtr();
  Serial.printf("RST_INFO = %lu\r\n", rsti->reason);
  Serial.print("RESET REASON => ");
  Serial.println(ESP.getResetReason());

  delay(1000);
  if (rsti->reason == REASON_DEEP_SLEEP_AWAKE) {
    ESP.reset();
  }
  else {
    ESP.deepSleep(1 * 10, WAKE_RF_DISABLED);
  }
}

void loop() {
}
