#include <SoftwareSerial.h>
SoftwareSerial bluetooth(2, 3);
void setup() {
  Serial.begin(9600);
  while (!Serial);
  bluetooth.begin(9600);
  Serial.println("\nCheck BLE Version 1.0.0\n");
}

void loop() {

  if (Serial.available()) {
    bluetooth.write(Serial.read());
  }
  if (bluetooth.available()) {
    Serial.write(bluetooth.read());
  }
}
