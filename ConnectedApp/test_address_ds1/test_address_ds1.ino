#include <OneWire.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // TX, RX

OneWire  ds(10);  // This is where DQ of your DS18B20 will connect.

void setup(void) {
  Serial.begin(9600);
  while (!Serial);
  mySerial.begin(9600);
  getDeviceAddress();
}

void getDeviceAddress(void) {
  byte i;
  byte addr[8];
  String data;
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

void loop(void) {
  if (mySerial.available()) {
    char check = mySerial.read();
    if (check == 'a') {
      Serial.println("Onclick A");
    }
    else if (check == 'b') {
      Serial.println("Onclick b");
    }
  }
}
