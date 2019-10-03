#include <SoftwareSerial.h>
#include <MCP4922.h>
#include <SPI.h>
SoftwareSerial bluetooth(2, 3);
MCP4922 DAC(11, 13, 10, 5);

int incomingByte;
String Text;
char get_Star = ' ';
int Read_command = 0;
int pin_number_digital = 0;
int pin_number_analog = 0;
int pin_state = 0;
int pin_value = 0;
int pin_value_state = 0;
int statuscase;
char send_status_pincheck;
int count = 0;
int pin_number;
#define Pin_chanal1 6
#define Pin_chanal2 7

unsigned char buffer[32] = "fsdf";

void setup() {
  Serial.begin(9600);  // Debug Serial
  SPI.begin();
  while (!Serial);
  bluetooth.begin(9600);   // Bluetooth Serial
  Serial.println("\nChat BLEMini Version\n");
  DAC.Set(2410, 0); //3.0 v
}

void loop() {

  if (Serial.available()) {
    bluetooth.write(Serial.read());
  }
  if (bluetooth.available()) {
    incomingByte = bluetooth.read();
    Serial.println(incomingByte);
    //bluetooth.write((const uint8_t*)buffer, 32);
    bluetooth.println(count);
    //bluetooth.print("x");
    //    switch (incomingByte) {
    //      case 0:
    //        DAC.Set(0, 0); //0.0 v
    //        for (int i = 0; i < len; i++) {
    //          buffer[i] = 1;
    //          bluetooth.write(buffer[i]);
    //          Serial.write(buffer[i]);
    //        }
    //        bluetooth.write((const uint8_t*)buffer, BUFFER_SIZE);
    //        Serial.println("CMD_11");
    //        break;
    //      case 1:
    //        DAC.Set(810, 0); //1.0 v
    //        Serial.println("CMD_22");
    //        break;
    //    }
  }
  //bluetooth.write((const uint8_t*)buffer, 32);
  //delay(500);
}

//#include <SoftwareSerial.h>
//SoftwareSerial soft(2, 3);   //RX, TX
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

//
//#include "EB_Bluetooth_4_0.h"
//#include <SoftwareSerial.h>
//#include <WString.h>
//#define LED 4 // Pin where Led is Connected
//#define RX 2 // Pin where Receive Serial Software BLE <-> Arduino
//#define TX 3 // Pin where Transmission Serial Software BLE <-> Arduino
//EB_Bluetooth_4_0 myBluetooth(RX, TX);
//void setup() {
//  Serial.begin(9600);
//  pinMode(LED, OUTPUT);
//  digitalWrite(LED, LOW);
//  myBluetooth.begin();
//  Serial.println("Waiting to be connected");
//}
//void loop() {
//  String command = "";
//  while (myBluetooth.available()) {
//    // while on serial is available data I will continue to concatenate data
//    command += (char)myBluetooth.read();
//    Serial.println(command);
//  }
//  if (command.length() == 2) {
//    switch (command.charAt(1)) {
//      case '0': // 0 means off -> LOW on Pin
//        digitalWrite(LED, LOW);
//        break;
//      case '1': // 1 means on -> HIGH on Pin
//        digitalWrite(LED, HIGH);
//        break;
//    }
//  }
//  delay(1000);
//}
