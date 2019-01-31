#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
PN532_SPI pn532spi(SPI, 16);
PN532 nfc(pn532spi);
uint8_t currentblock = 1;
uint8_t data[16];

void setup() {
  Serial.begin(9600);
  Serial.println("Arduino_test_write_data_mifare!");
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  nfc.SAMConfig();
  Serial.println("Waiting for an ISO14443A Card ...");
}

void loop() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
  }
  if (uidLength == 4) {
    Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
    Serial.println("Trying to authenticate block 4 with default KEYA value");
    uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, currentblock, 1, keya);
    if (success) {
      Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
      success = nfc.mifareclassic_ReadDataBlock(currentblock, data);
      if (success) {
        Serial.print("Reading Block :");
        Serial.println(currentblock);
        nfc.PrintHexChar(data, 16);
        Serial.println("");
        delay(1000);
      }
      else {
        Serial.println("Ooops ... unable to read the requested block.  Try another key?");
      }
    }
  }
  delay(100);
}
