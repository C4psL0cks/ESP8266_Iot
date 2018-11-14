//#include <Wire.h>
//#include <PN532_I2C.h>
//#include <PN532.h>
//
//PN532_I2C pn532i2c(Wire);
//PN532 nfc(pn532i2c);

#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
PN532_SPI pn532spi(SPI, 16);
PN532 nfc(pn532spi);


uint8_t currentblock = 1;             // block write select
uint8_t data[20];                     // block write data
bool Keycard = false;
uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uidLength;

void setup() {
  Serial.begin(115200);
  nfc.begin();
  nfc.SAMConfig();
  Serial.println("Start Read...");
}

void loop() {
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    if (uidLength == 4) {
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };                                     // key
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, currentblock, 1, keya);         // select block
      if (success) {
        success = nfc.mifareclassic_ReadDataBlock(currentblock, data);                              // Readblock
        if (success) {
          Serial.println();
          char check[20];
          String Text;
          int num;
          for (int i = 0 ; i < sizeof(data); i++ ) {
            String text = String(data[i], DEC);
            num = text.toInt();
            check[i] = num;
            Text += check[i];
          }
          if (Text == "ADMIN") {
            Keycard = true;
            Serial.print("ADMIN!!");
          }
          else {
            Keycard = false;
            Serial.print("UNKNOW!!");
          }
        }
      }
    }
    else {
      Keycard = false;
    }
  }
  else {
    Serial.println("Wait New Read!!");
  }
  delay(100);

}
