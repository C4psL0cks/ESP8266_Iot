// Example By ArduinoAll
// more info https://www.arduinoall.com/b/141
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // TX, RX
void setup()
{
  Serial.begin(38400);
  while (!Serial);
  mySerial.begin(38400);
}
void loop()
{
  if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available())
    mySerial.write(Serial.read());
}
