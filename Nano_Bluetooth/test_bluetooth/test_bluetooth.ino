///---- read serial ----
//char BYTE;
//
//void setup() {
//  Serial.begin(9600);
//}
//
//void loop() {
//  while (!Serial.available());   // stay here so long as COM port is empty
//  BYTE = Serial.read();        // read next available byte
//  Serial.print(BYTE);
//  delay(50);
//}
//--- run AT

//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(2, 3); // TX, RX
//void setup()
//{
//  Serial.begin(38400);
//  while (!Serial);
//  mySerial.begin(38400);
//  Serial.println("test");
//}
//void loop()
//{
//  //Serial.println("test");
//  if (mySerial.available())
//    Serial.write(mySerial.read());
//  if (Serial.available())
//    mySerial.write(Serial.read());
//}

//
//#include <SoftwareSerial.h>
//int bluetoothTx = 2;  // TX-O pin of bluetooth mate
//int bluetoothRx = 3;  // RX-I pin of bluetooth mate
//SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);   //TX, RX
//
//char get_char = ' ';  //read serial
//char test = ' ';
//void setup() {
//  Serial.begin(9600);  // Debug Serial
//  bluetooth.begin(9600);   // Bluetooth Serial
//  //setupBluetooth();
//  Serial.println("Hello.");
//}
//
//void loop() {
//
//  // wait for incoming data
//  //if (Serial.available() < 1) return; // if serial empty, return to loop().
//  // parse incoming command start flag
//
//  if (Serial.available()) {
//    Serial.println("IF1: ");
//    bluetooth.write(Serial.read());
//    Serial.println();
//  }
//  if (bluetooth.available()) {
//    Serial.println("IF2: ");
//    Serial.write(bluetooth.read()); // รับค่าจากโทรสับแสดงออก serail
//    test = bluetooth.read();
//    Serial.println(test);
//    if (bluetooth.read()) {
//      char buf[20] = "ans";
//      bluetooth.print(buf);
//    }
//  }
//}

//// init for Sparkfun radio (works for BLEmini too)
//void setupBluetooth() {
//  bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
//  bluetooth.print("$$$");  // Enter command mode
//  delay(100);  // Short delay, wait for the Mate to send back CMD
//  bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
//  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
//  bluetooth.begin(9600);  // Start bluetooth serial at 9600
//}


//
#include <SoftwareSerial.h>
SoftwareSerial soft(2, 3);   //TX, RX

void setup() {
  Serial.begin(9600);  // Debug Serial
  soft.begin(9600);   // Bluetooth Serial
}

void loop() {
  if (Serial.available()) {
    soft.write(Serial.read());
  }
  if (soft.available()) {
    Serial.write(soft.read());
    if(soft.read()){
      Serial.println("ANS");
    }
  }
}
