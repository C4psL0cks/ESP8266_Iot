
//checkESP_Reset()
#include <Arduino.h>
#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif
//checkChipID
#include <ESP8266WiFi.h>
//checkI2C
#include <Wire.h>
// ACC_MODE
ADC_MODE(ADC_VCC);

void setup() {
  Serial.begin (115200);
  while (!Serial) {}
  checkESP_Reset();
  checkChipID();
  checkI2C();
}
void loop() {}

void checkESP_Reset() {
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
void checkChipID() {
  Serial.println ();
  Serial.printf(" ESP8266 Chip id = %08X\n", ESP.getChipId());
}
void checkI2C() {
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  Wire.begin();
  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
    {
      Serial.print("Found address: ");
      Serial.print(i, DEC);
      Serial.print(" (0x");
      Serial.print(i, HEX);
      Serial.println (")");
      count++;
      delay (1);
    }
  }
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}
void ACC_MODE() {
  int sensorValue = ESP.getVcc();
  float volts = (float)sensorValue / 1023.00;
  Serial.println(volts);
  delay(100);
}
