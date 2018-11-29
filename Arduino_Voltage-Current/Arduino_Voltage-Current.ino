
/* Connection pins:

  Arduino     Current Sensor B43
  A0                  VT
  A1                  AT
  +5V                 VIN
  +5V                 VOUT
  GND                 GND
*/

#include <Wire.h>
#define VT_PIN A0 // connect VT
#define AT_PIN A1// connect AT

#define ARDUINO_WORK_VOLTAGE 5.0

void setup()
{
  Serial.begin(9600);
  Serial.println("Voltage (V) / Current (A)");
}

void loop()
{
  int vt_temp = analogRead(VT_PIN);
  int at_temp = analogRead(AT_PIN);

  double voltage = vt_temp * (ARDUINO_WORK_VOLTAGE / 1023.0) * 5;
  double current = at_temp * (ARDUINO_WORK_VOLTAGE / 1023.0);
  Serial.print("Analog0: "); Serial.print(vt_temp); Serial.print(" / "); Serial.print("Analog1: "); Serial.print(at_temp);  Serial.print(" ----- ");
  Serial.print(voltage); Serial.print(" / "); Serial.println(current);
  delay(1000);
}
