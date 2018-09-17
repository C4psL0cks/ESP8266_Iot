#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <TridentTD_LineNotify.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial
#define BLYNK_MAX_SENDBYTES 256
#define closed 0
#define opened 1
#define VT_PIN A0
#define AT_PIN A0
#define ARDUINO_WORK_VOLTAGE 3.3
#define LedGpioPinRed 8
#define LedGpioPinBlue 9
#define RelayGpioPin 10
#define DoorGpioPin 11
#define BuzzerGpioPin 12
#define DoorUnLockGpioPin 13

Adafruit_MCP23017 mcp;
PN532_SPI pn532spi(SPI, 16);
PN532 nfc(pn532spi);

WiFiManager wifiManager;
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0); // switch
  Blynk.syncVirtual(V1); // led status close
  Blynk.syncVirtual(V2); // led status open
  Blynk.syncVirtual(V3); // volt
  Blynk.syncVirtual(V4); // timer
  Blynk.syncVirtual(V5); // door status
  Blynk.syncVirtual(V6); // lock status
}
WidgetLED Ledclose(V1);
WidgetLED Ledopen(V2);

uint8_t currentblock = 1;             // block write select
uint8_t data[20];                     // block write data
bool shouldSaveConfig = false;
int ButtonStatelock;
int DoorState = 0;
int Door = 0;
int Relay = 0;
int Keycard = 2;
int vt_temp;
double voltage;
char blynk_token[35] = "";
char line_token[45] = "";
char name_device[35] = "";
char ssid[20] = "";
char pass[20] = "";

void setup() {

  Serial.begin(115200);

  SPIFFS.format();

  mcp.begin();
  mcp.pinMode(LedGpioPinRed, OUTPUT);
  mcp.digitalWrite(LedGpioPinRed, HIGH);
  mcp.pinMode(LedGpioPinBlue, OUTPUT);
  mcp.digitalWrite(LedGpioPinBlue, HIGH);
  mcp.pinMode(RelayGpioPin, OUTPUT);
  mcp.digitalWrite(RelayGpioPin, LOW);
  mcp.pinMode(BuzzerGpioPin, OUTPUT);
  mcp.digitalWrite(BuzzerGpioPin, LOW);
  mcp.pinMode(DoorGpioPin, INPUT);
  mcp.pullUp(DoorGpioPin, HIGH);
  mcp.pinMode(DoorUnLockGpioPin, INPUT);
  mcp.pullUp(DoorUnLockGpioPin, HIGH);
  Ledclose.off();
  Ledopen.off();
  Relayclose();

  Serial.println("mounting FS...");
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(blynk_token, json["blynk_token"]);
          strcpy(line_token, json["line_token"]);
          strcpy(name_device, json["name_device"]);
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 35);
  WiFiManagerParameter custom_line_token("line", "line token", line_token, 45);
  WiFiManagerParameter custom_name_device("name", "name device", name_device, 35);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_blynk_token);
  wifiManager.addParameter(&custom_line_token);
  wifiManager.addParameter(&custom_name_device);

  wifiManager.resetSettings();

  if (!wifiManager.autoConnect("SMART LOCK ALERT")) {
  }
  Serial.println("connected...success :)");
  strcpy(blynk_token, custom_blynk_token.getValue());
  strcpy(line_token, custom_line_token.getValue());
  strcpy(name_device, custom_name_device.getValue());

  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    json["blynk_token"] = blynk_token;
    json["line_token"] = line_token;
    json["name_device"] = name_device;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }
    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
  }
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  Blynk.begin(blynk_token, ssid, pass);
  LINE.setToken(line_token);

  nfc.begin();
  nfc.SAMConfig();
  Serial.println("Waiting for Card ...");
}
void loop() {

  Blynk.run();
  vt_temp = analogRead(VT_PIN);
  voltage = vt_temp * (ARDUINO_WORK_VOLTAGE / 1023.0) * 5;
  Blynk.virtualWrite(V0, Door);
  Blynk.virtualWrite(V3, voltage);
  Blynk.virtualWrite(V5, Door);
  Blynk.virtualWrite(V6, Relay);

  RFID();

  if (mcp.digitalRead(DoorGpioPin) == LOW && DoorState == opened) { // ปิดประตู
    DoorState = closed;
    Door = 0;
    Relayclose();
    Buzzeroff();
    LedBlueon();
    LedRedoff();
    LINE.notify(String(name_device) + "-->DOOR CLOSE!!");
    Serial.println("@@CLOSE");
  }
  if (mcp.digitalRead(DoorGpioPin) == HIGH && DoorState == closed) { // เปิดประตู
    DoorState = opened;
    Door = 1;
    Buzzeron();
    LedRedon();
    LedBlueoff();
    LINE.notify(String(name_device) + "-->DOOR OPEN!!");
    Serial.println("@@OPEN");
  }





  // Attack
  //If the door is open by the door, the lock is also locked. to notify and lock the door .
  //  if (Door == 1 && Relay == 0) {
  //    Buzzeron();
  //    Relayclose();
  //    Serial.println("@@Notify Attack Door CASE 1: " + String(Door) + " Relay :" + String(Relay));
  //    //LINE.notify(String(name_device) + "--> ATTACK DOOR OPEN!!");
  //  }
  //Do not know the key card.
  //don't know the key card. The door is locked. to notify and lock the door .
  //  if (Keycard == 1 && Relay == 0) {
  //    Buzzeron();
  //    Keycard = 2;
  //    Serial.println("@@Notify Keycard + Realy CASE 1: " + String(Keycard) + " Relay :" + String(Relay));
  //  }
  //don't know the key card. the door is closed and the door is locked to notify. and lock the door . ******
  //  if (Keycard == 1 && Door == 0 && Relay == 0) {
  //    Buzzeron();
  //    Relayclose();
  //    Keycard = 2;
  //    Serial.println("@@Notify Keycard + Door + Realy CASE 2: " + String(Keycard) + " Door : " + String(Door) + "Relay :" + String(Relay));
  //  }
  //  //don't know the key card. attack the door and door lock to alert.
  //  if (Keycard == 1 && Door == 1 && Relay == 0) {
  //    Buzzeron();
  //    Relayclose();
  //    Keycard = 2;
  //    Serial.println("@@Notify Keycard + Door + Realy CASE 3: " + String(Keycard) + " Door : " + String(Door) + "Relay :" + String(Relay));
  //  }


  //Open Normal
  //Take the key card, tap the door, lock it, not alert and unlock the door.
  //  if (Keycard == 0 && Relay == 0) {
  //    Buzzeroff();
  //    Relayopen();
  //    delay(1000);      // delay
  //    Relayclose();
  //    Keycard = 2;      // Set keycard = 2 because door lock
  //    Serial.println("@@Normal Open CASE 1: " + String(Keycard) + "Relay :" + String(Relay));
  //  }
  //  //Take the key card to the door, lock the door, close the door, not to alert and unlock the door.
  //  if (Keycard == 0 && Door == 0 && Relay == 0) {
  //    Buzzeroff();
  //    Relayopen();
  //    delay(1000);      //delay
  //    Relayclose();
  //    Keycard = 2;      // Set keycard = 2 because door lock
  //    Serial.println("@@Normal Open CASE 2: " + String(Keycard) + " Door : " + String(Door) + "Relay :" + String(Relay));
  //  }

  //
  //  //App
  //  //Unlock locked door without notification and unlock door.
  //  if (ButtonStatelock == 1 && Relay == 0 && Door == 0) {
  //    Buzzeroff();
  //    Relayopen();
  //    delay(1000);
  //    Relayclose(); //เดียวมัน set ค่ากลับเอง
  //    Serial.println("@@Normal App Open CASE 1: " + String(ButtonStatelock) + "Relay :" + String(Relay));
  //  }
  //  //Unlock the door, lock the door, close the door without warning and unlock the door.
  //  if (ButtonStatelock == 0 && Relay == 1 && Door == 1) {
  //    Buzzeroff();
  //    Relayopen();
  //    delay(1000);
  //    Relayclose(); //เดียวมัน set ค่ากลับเอง
  //    Serial.println("@@Normal App Open CASE 2: " + String(ButtonStatelock) + "Relay :" + String(Relay));
  //  }
}
BLYNK_WRITE(V0) {
  ButtonStatelock = param.asInt();
  if (ButtonStatelock == 0) {
    Relayclose();
    Buzzeroff();
    LedBlueon();
    LedRedoff();
    LINE.notify(String(name_device) + "-->DOOR CLOSE!!");
  }
  if (ButtonStatelock == 1) {
    Relayopen();
    Buzzeron();
    LedRedon();
    LedBlueoff();
    LINE.notify(String(name_device) + "-->DOOR OPEN!!");
  }
  if (ButtonStatelock == 0 && Door == 1 && Relay == 0) {
    Buzzeron();
    LINE.notify(String(name_device) + "--> ATTACK DOOR OPEN!!");
  }
  if (ButtonStatelock == 0 && Keycard == 1 && Relay == 0) {
    Buzzeron();
    Keycard = 2;
    Serial.println("@@Notify Keycard + Realy CASE 1: " + String(Keycard) + " Relay :" + String(Relay));
  }
}
//// Normal
//BLYNK_WRITE(V0) {
//  ButtonStatelock = param.asInt();
//  if (ButtonStatelock == 0) {
//    Relayclose();
//    Buzzeroff();
//    LedBlueon();
//    LedRedoff();
//    LINE.notify(String(name_device) + "-->DOORCLOSE!!");
//  }
//  if (ButtonStatelock == 1) {
//    Relayopen();
//    Buzzeron();
//    LedRedon();
//    LedBlueoff();
//    LINE.notify(String(name_device) + "-->DOOROPEN!!");
//  }
//}
//// timer
//BLYNK_WRITE(V4) {
//  ButtonStatelock = param.asInt();
//  if (ButtonStatelock == 0) {
//    Relayclose();
//    LedBlueon();
//    LedRedoff();
//    LINE.notify(String(name_device) + " CLOSE");
//  }
//  if (ButtonStatelock == 1) {
//    Relayopen();
//    LedRedon();
//    LedBlueoff();
//    LINE.notify(String(name_device) + " OPEN");
//  }
//}
void Relayclose() {
  Relay = 0;
  mcp.digitalWrite(RelayGpioPin, LOW); // ทำงาน ปิดประตู
}
void Relayopen() {
  Relay = 1;
  mcp.digitalWrite(RelayGpioPin, HIGH); // ไม่ทำงาน เปิดประตู
}
void Buzzeron() {
  mcp.digitalWrite(BuzzerGpioPin, HIGH);
  delay(200);
  mcp.digitalWrite(BuzzerGpioPin, LOW);
  delay(200);
}
void Buzzeroff() {
  mcp.digitalWrite(BuzzerGpioPin, LOW);
}
void LedRedon() {
  Ledopen.on();
  Ledclose.off();
  mcp.digitalWrite(LedGpioPinRed, HIGH);
}
void LedRedoff() {
  Ledopen.off();
  Ledclose.on();
  mcp.digitalWrite(LedGpioPinRed, LOW);
}
void LedBlueon() {
  Ledclose.on();
  Ledopen.off();
  mcp.digitalWrite(LedGpioPinBlue, HIGH);
}
void LedBlueoff() {
  Ledclose.off();
  Ledopen.on();
  mcp.digitalWrite(LedGpioPinBlue, LOW);
}
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void RFID() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    Serial.print("Read Card");
  }
  if (uidLength == 4) {
    uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // key
    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, currentblock, 1, keya); // select block
    if (success) {
      success = nfc.mifareclassic_ReadDataBlock(currentblock, data); // Readblock
      if (success) {
        Serial.println();
        char check[20];
        int num;
        String Text;
        for (int i = 0 ; i < sizeof(data); i++ ) {
          String text = String(data[i], DEC);
          num = text.toInt();
          check[i] = num;
          Text += check[i];
        }
        if (Text == "ADMIN") {
          Keycard = 0;
          LINE.notify(String(name_device) + " Access by Admin!!");
          Serial.print("ADMIN!!");
        }
        else if (Text == "") {
          Keycard = 1;
          LINE.notify(String(name_device) + " Access by Unknow!!");
          Serial.print("UNKNOW!!");

        } else {
          Keycard = 1;
          LINE.notify(String(name_device) + " Access by Unknow!!");
          Serial.print("UNKNOW!!");
        }
        Serial.println();
      }
      else {
        Serial.println("Ooops ..");
      }
    }
  }
  else {
    Serial.println("**Start Wait key Card **");
  }
  delay(500);
}
