// Serial.println("Start...Setup Buletooth");
//
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


// Serial.println("Start...Setup Buletooth");
#include <SoftwareSerial.h>
SoftwareSerial bluetooth(2, 3);   //TX, RX

void setup() {
  Serial.begin(9600);  // Debug Serial
  while (!Serial);
  bluetooth.begin(9600);   // Bluetooth Serial
  Serial.println("Start...Read Buletooth");
}

void loop() {

  if (bluetooth.available()) {
    while (bluetooth.available()) {
      Serial.write(bluetooth.read());
    }
  }

  //  if (bluetooth.available()) {
  bluetooth.print("ans");
  //  }
  delay(1000);
}


// Check buff
//#include <SoftwareSerial.h>
//int bluetoothTx = 2;  // TX-O pin of bluetooth mate
//int bluetoothRx = 3;  // RX-I pin of bluetooth mate
//SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);   //TX, RX
//
//void setup() {
//  Serial.begin(9600);  // Debug Serial
//  while (!Serial);
//  bluetooth.begin(9600);   // Bluetooth Serial
//  Serial.println("Hello.");
//}
//
//void loop() {
//
//  if (Serial.available()) {
//    //bluetooth.write(Serial.read());
//  }
//  if (bluetooth.available()) {
//    Serial.write(bluetooth.read()); // รับค่าจากโทรสับแสดงออก serail
//    bluetooth.write("a\n");
//  }
//}
