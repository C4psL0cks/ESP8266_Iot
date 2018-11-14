#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <Servo.h>
Servo myservo;

#include <TridentTD_LineNotify.h>

#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
PN532_SPI pn532spi(SPI, 16);
PN532 nfc(pn532spi);

uint8_t currentblock = 1;
uint8_t data[20];
uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uidLength;

bool Keycard = false;
bool Switch = false;
bool Door = true;
bool Lock = true;

int ButtonStatelock;
int ButtonStatetimer;
const int switchpin = 4;
const int doorpin  = 5;
const int buzzerpin = 2;
const int servopin = 15;
int doorState = 0;
#define closed 0
#define opened 1

char line_token[45] = "lTdXXOfUCH00vgQWgjRNdt83bVptSk0yQysFezcVIvp";
char auth[] = "2650f83c3ad244b0b844e01e4611072e";
char ssid[] = "6021607";
char pass[] = "17401449";

ADC_MODE(ADC_VCC);

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0); // servo
  Blynk.syncVirtual(V1); // timer
  Blynk.syncVirtual(V2); // volt
  Blynk.syncVirtual(V3); // door status
  Blynk.syncVirtual(V4); // lock status
}
BLYNK_WRITE(V0) {
  ButtonStatelock = param.asInt();
  Serial.println("LOCK :" + String(ButtonStatelock));
  if (ButtonStatelock == 1) { // false

    DoorLock();
    LINE.notify("LOCK THE DOOR");
    Blynk.notify("LOCK THE DOOR");
  }
  if (ButtonStatelock == 0) { // true
    DoorUnLock();
    LINE.notify("UNLOCK THE DOOR");
    Blynk.notify("UNLOCK THE DOOR");
  }
}
BLYNK_WRITE(V1) {
  ButtonStatetimer = param.asInt();
  Serial.println("TIMER :" + String(ButtonStatetimer));
  if (ButtonStatetimer == 0) {
    DoorLock();
    Blynk.notify("TIMER LOCK THE DOOR");
  }
  if (ButtonStatetimer == 1) {
    DoorUnLock();
    Blynk.notify("TIMER UNLOCK THE DOOR");
  }
}
void setup() {

  Serial.begin(115200);
  pinMode(switchpin, INPUT);
  pinMode(doorpin, INPUT);
  pinMode(buzzerpin, OUTPUT);
  myservo.attach(servopin);
  DoorLock();
  Buzzeroff();
  //
  Blynk.begin(auth, ssid, pass);
  LINE.setToken(line_token);

  nfc.begin();
  nfc.SAMConfig();
  Serial.println("Start Read...");
}

void loop() {

  Blynk.run();

  int sensorValue = ESP.getVcc();
  float volts = (float)sensorValue / 1023.00;
  Blynk.virtualWrite(0, ButtonStatelock);
  Blynk.virtualWrite(2, volts);
  Blynk.virtualWrite(3, Door);
  Blynk.virtualWrite(4, Lock);

  RFID();


  if ( digitalRead(doorpin) == HIGH && doorState == opened) {
    Blynk.notify("Close");
    LINE.notify("Close");
    //Serial.println("Close");
    doorState = closed;
    Door = true; // defult = true
  }
  if ( digitalRead(doorpin) == LOW && doorState == closed) {
    Blynk.notify("Open");
    LINE.notify("Open");
    //Serial.println("Open");
    doorState = opened;
    Door = false;  // defult = true in if chang to false
  }
  if (digitalRead(switchpin) == HIGH) {
    Switch = true;  // defult = false in if chang to true
  }
  if (digitalRead(switchpin) == LOW) {
    Switch = false; // defult = false
  }

  // open the door by the door lock
  if ((Door == false) && (Lock == true)) {
    Buzzeron();
    Blynk.notify("Attack Alert");
    LINE.notify("Attack Alert");
    //Serial.println("Attack Alert");
    delay(3000);
  }

  if ((ButtonStatelock == false ) && (Door == true) && ( Lock == true)) {
    Blynk.notify("Access by App");
    LINE.notify("Access by App");
    //Serial.println("Access by App");
    Buzzeroff();
    DoorUnLock(); // set Lock = false
    delay(3000);
    DoorLock(); //set Lock = true
  }
  if ((Keycard == true ) && (Door == true) && ( Lock == true)) {
    Blynk.notify("Access by keycard");
    LINE.notify("Access by keycard");
    //Serial.println("Access by keycard");
    Buzzeroff();
    DoorUnLock(); // set Lock = false
    delay(3000);
    DoorLock(); //set Lock = true
    Keycard = false; // set from true to false out if
  }

  //NO case True any
  if ((Switch == true) && (Door == true) && ( Lock == true) ) {
    //Blynk.notify("Onclick in Home");
    //Serial.println("Onclick in Home");
    Buzzeroff();
    DoorUnLock();// set Lock = false
    delay(3000);
    DoorLock(); //set Lock = true
    Switch = false; // set from true to false out if
  }
}
void DoorLock() {
  Lock = true;  // defult = true
  myservo.write(110);
}
void DoorUnLock() {
  Lock = false;  // defult = true in function chang to false
  myservo.write(40);
}
void Buzzeron() {
  digitalWrite(buzzerpin, LOW);
}
void Buzzeroff() {
  digitalWrite(buzzerpin, HIGH);
}
void RFID() {
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
            //Serial.print("ADMIN!!");
            Blynk.notify("ADMIN!! UNLOCK THE DOOR");
            LINE.notify("ADMIN!! UNLOCK THE DOOR");
          }
          else {
            Keycard = false;
            Buzzeron();
            //Serial.print("UNKNOW!!");
            Blynk.notify("UNKNOW!! TRY TO ACCESS THE DOOR");
            LINE.notify("UNKNOW!! TRY TO ACCESS THE DOOR");
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
  //delay(100);
}
