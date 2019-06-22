#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>


uint8_t data[20]= "ADMIN";
bool authenticated = false;
uint8_t currentblock = 1;
uint8_t keyuniversal[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

void setup()
{
  Serial.begin(9600);
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
      Serial.print(currentblock);
      Serial.println("-------------------------");
      success = nfc.mifareclassic_AuthenticateBlock (uid, uidLength, currentblock, 1, keyuniversal);
      if (success) {
        Serial.println("Authentication complete");
        //ถ้าเท่ากับ 4 คือบัตร MIFARE Classic ถ้าในครั้งแรก คือตัวแปร authenticated เป็น false ส่งคำสั่ง 
        //AuthenticateBlock พร้อมกำหนดพารามิเตอร์ รหัส UID ,ขนาดรหัสUID,Block ที่ต้องการเขียน ,Key A กำหนดเป็น 1 และรหัส Key A 
        success = nfc.mifareclassic_WriteDataBlock(1, data);
        if (success) {
          Serial.println("Write data complete");
        }
        else {
          Serial.println("Write data fail!");
        }
      }
      else {
        Serial.println("Authentication");
      }
    }
  }
}
