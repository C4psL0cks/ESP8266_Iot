#include <SoftwareSerial.h>
SoftwareSerial soft(2, 3);   //TX, RX

void setup() {
  Serial.begin(9600);  // Debug Serial
  soft.begin(9600);   // Bluetooth Serial
}

void loop() {

  if (soft.available()) {
    Serial.write(soft.read()); // รับค่าจากโทรสับแสดงออก serail
    if (soft.read()) {
      int x = 11;
      soft.println(x);
    }
  }
}


//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(2, 3); // TX, RX
//void setup()
//{
//  Serial.begin(38400);
//  while (!Serial);
//  mySerial.begin(38400);
//}
//void loop()
//{
//  if (mySerial.available())
//    Serial.write(mySerial.read());
//  if (Serial.available())
//    mySerial.write(Serial.read());
//}
