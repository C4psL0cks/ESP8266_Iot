#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // TX, RX
void setup()
{
  Serial.begin(9600);
  while (!Serial);
  mySerial.begin(9600);
}
void loop()
{
  if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }

}
