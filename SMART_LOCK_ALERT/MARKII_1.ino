#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
#include <TridentTD_LineNotify.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
ADC_MODE(ADC_VCC);
uint8_t currentblock = 1;
uint8_t data[20];
uint8_t success;
uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
uint8_t uidLength;
PN532_SPI pn532spi(SPI, 16);
PN532 nfc(pn532spi);

Servo myservo;

char auth[] = "595f31446d1f4567b77d2d425b9011ea";
char line_token[45] = "lTdXXOfUCH00vgQWgjRNdt83bVptSk0yQysFezcVIvp";
char ssid[] = "6021607";
char pass[] = "17401449";
int buttonStatelock;
bool Keycard = false;

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
}
BLYNK_WRITE(V5)
{
  buttonStatelock = param.asInt();
  if (buttonStatelock == 1)
  {
    myservo.write(0);
    delay(1000);
    myservo.write(140);
    Blynk.notify("LOCK THE DOOR");
    LINE.notify("LOCK THE DOOR");
  }
}
void setup()
{
  Serial.begin(115200);
  myservo.attach(5);
  myservo.write(130);
  Blynk.begin(auth, ssid, pass);
  LINE.setToken(line_token);

  nfc.begin();
  nfc.SAMConfig();
  Serial.println("Start Read...");
}
void loop()
{

  Blynk.run();
  RFID();
  int sensorValue = ESP.getVcc();
  float volts = (float)sensorValue / 1023.00;
  Blynk.virtualWrite(V4, volts);
  Blynk.virtualWrite(V5, buttonStatelock);
}
void RFID()
{
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success)
  {
    if (uidLength == 4)
    {
      uint8_t keya[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};                               // key
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, currentblock, 1, keya); // select block
      if (success)
      {
        success = nfc.mifareclassic_ReadDataBlock(currentblock, data); // Readblock
        if (success)
        {
          Serial.println();
          char check[20];
          String Text;
          int num;
          for (int i = 0; i < sizeof(data); i++)
          {
            String text = String(data[i], DEC);
            num = text.toInt();
            check[i] = num;
            Text += check[i];
          }
          if (Text == "ADMIN")
          {
            Keycard = true;
            Serial.println("ADMIN!!");
            Blynk.notify("ADMIN!! UNLOCK THE DOOR");
            LINE.notify("ADMIN!! UNLOCK THE DOOR");
          }
          else
          {
            Keycard = false;
            Serial.println("UNKNOW!!");
            Blynk.notify("UNKNOW!! TRY TO ACCESS THE DOOR");
            LINE.notify("UNKNOW!! TRY TO ACCESS THE DOOR");
          }
        }
      }
    }
    else
    {
      Keycard = false;
    }
  }
  else
  {
    Serial.println("Wait New Read!!");
  }
  delay(100);
}
