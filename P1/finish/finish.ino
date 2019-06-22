#include <SoftwareSerial.h>
#include <OneWire.h>
SoftwareSerial mySerial(2, 3);   //TX, RX
OneWire  ds(10);

int in1 = 5;
int in2 = 6;
int sw = 9;
String data;
String check = "233";
String chk = "";
int x;
void setup(void) {
  Serial.begin(9600);
  mySerial.begin(9600);   // Bluetooth Serial
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(sw, INPUT);
  digitalWrite(sw, HIGH);
}

void loop(void) {

  if (mySerial.available()) {
    chk = mySerial.read();
    Serial.print(chk);
  }

  if (chk == check) {
    Serial.println("OK");
  }

  if (digitalRead(sw) == LOW) {
    getDeviceAddress();
    foward();
    delay(500);
    black();
    delay(500);
    stop();
  }
  data = "";

}
void getDeviceAddress(void) {
  byte i;
  byte addr[8];
  Serial.println("Getting the address...\n\r");
  while (ds.search(addr)) {
    Serial.print("The address is:\t");
    for ( i = 0; i < 8; i++) {
      Serial.print(addr[i]);
      data += addr[i];
    }
    if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n");
      return;
    }
  }
  Serial.println();
  Serial.println("DATA :" + data);
  ds.reset_search();
  return;
}
void foward() {
  analogWrite(in1, 0);
  analogWrite(in2, 400);
}
void black() {
  analogWrite(in1, 400);
  analogWrite(in2, 0);
}
void stop() {
  analogWrite(in1, 0);
  analogWrite(in2, 0);
}
