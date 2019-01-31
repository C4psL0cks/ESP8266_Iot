#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);

uint8_t data[16] = "";  /// data
bool authenticated = false;
uint8_t currentblock = 1;
uint8_t keyuniversal[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };  /// key
int ledpass = 7;
int ledfail = 6;
int Status = 5;
void setup()
{
  Serial.begin(9600);
  pinMode(ledpass, OUTPUT);
  pinMode(ledfail, OUTPUT);
  pinMode(Status, OUTPUT);
  digitalWrite(ledpass, LOW);
  digitalWrite(ledfail, LOW);
  Serial.println("Arduino_test_write_data_mifare!");
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata)  {
    Serial.print("Didn't find PN53x board");
    while (1);
  }
  nfc.SAMConfig();
  Serial.println("Waiting for an ISO14443A Card ...");
}

void loop() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success)  {
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
  }
  if (uidLength == 4) {
    Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
    if (!authenticated) {
      Serial.print("------------------------Sector ");
      Serial.print(currentblock); // เลือก บล๊อกที่จะใส่ข้อมูล
      Serial.println("-------------------------");
      success = nfc.mifareclassic_AuthenticateBlock (uid, uidLength, currentblock, 1, keyuniversal);
      if (success) {
        Serial.println("Authentication complete");
        success = nfc.mifareclassic_WriteDataBlock(currentblock, data);
        if (success) {
          Serial.println("Write data complete");
          Serial.println("-------------------------");
          Serial.print("DATAWRITE :"); Serial.write(data, 16);
          Serial.println();
          Serial.println("-------------------------");
          digitalWrite(ledpass, HIGH);
          delay(2000);
          digitalWrite(ledpass, LOW);
        }
        else {
          Serial.println("Write data fail!");
          Serial.println("-------------------------");
          Serial.print("DATAWRITE :"); Serial.write(data, 16);
          Serial.println();
          Serial.println("-------------------------");
          digitalWrite(ledfail, HIGH);
          delay(2000);
          digitalWrite(ledfail, LOW);
        }
      }
      else {
        Serial.println("Authentication wait");
        digitalWrite(Status, HIGH);
      }
    }
  }
  //while(1);
  delay(1000);
}
