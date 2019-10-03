// HC-05 Setup
//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(2, 3); // TX, RX
//void setup()
//{
//  Serial.begin(38400);
//  while (!Serial);
//  mySerial.begin(38400);
//  Serial.println("Start...Setup Buletooth");
//}
//void loop()
//{
//  //Serial.println("test");
//  if (mySerial.available())
//    Serial.write(mySerial.read());
//  if (Serial.available())
//    mySerial.write(Serial.read());
//}

// Read HC-05
#include <SoftwareSerial.h>
int bluetoothTx = 2;
int bluetoothRx = 3;
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  bluetooth.begin(9600);
 Serial.println("\nChat HC-05 Version\n");
}

void loop() {

  bluetooth.listen();
  if (Serial.available()) {
    bluetooth.write(Serial.read());
  }
  if (bluetooth.available()) {
    Serial.write(bluetooth.read()); // รับค่าจากโทรสับแสดงออก serail
    //bluetooth.print("a");
  }
}


//HM-10 Setup
//#include <SoftwareSerial.h>
//SoftwareSerial soft(2, 3);
//
//void setup() {
//  Serial.begin(9600);  // Debug Serial
//  soft.begin(9600);   // Bluetooth Serial
//}
//
//void loop() {
//  if (Serial.available()) {
//    soft.write(Serial.read());
//  }
//  if (soft.available()) {
//    Serial.write(soft.read());
//  }
//}

//#include <SoftwareSerial.h>
//SoftwareSerial soft(2, 3);
//void setup() {
//  Serial.begin(9600);  // Debug Serial
//  while (!Serial);
//  soft.begin(9600);   // Bluetooth Serial
//  Serial.println("\nChat BLEMini Version\n");
//}
//
//void loop() {
//
//  if (Serial.available()) {
//    soft.write(Serial.read());
//  }
//  if (soft.available()) {
//    Serial.write(soft.read());
//  }
//}
