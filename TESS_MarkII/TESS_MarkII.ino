#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TridentTD_LineNotify.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"

#define BLYNK_PRINT Serial
#define VT_PIN A0
#define AT_PIN A0
#define ARDUINO_WORK_VOLTAGE 3.3
#define closed 0
#define opened 1
#define LedGpioPinRed 8
#define LedGpioPinBlue 9
#define RelayGpioPin 10
#define DoorGpioPin 11
#define BuzzerGpioPin 12

uint8_t currentblock = 1;
uint8_t data[20];
bool shouldSaveConfig = false;
int ButtonStatelock, ButtonStatelocks;
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

Adafruit_MCP23017 mcp;
PN532_SPI pn532spi(SPI, 16);
PN532 nfc(pn532spi);
WiFiManager wifiManager;
WidgetLED Ledclose(V1);
WidgetLED Ledopen(V2);

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0); // switch
  Blynk.syncVirtual(V1); // led status close
  Blynk.syncVirtual(V2); // led status open
  Blynk.syncVirtual(V3); // volt
  Blynk.syncVirtual(V4); // timer
  Blynk.syncVirtual(V5); // door status
  Blynk.syncVirtual(V6); // lock status
}

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
  mcp.digitalWrite(BuzzerGpioPin, HIGH);
  mcp.pinMode(DoorGpioPin, INPUT);
  mcp.pullUp(DoorGpioPin, HIGH);
  Ledclose.on();
  Ledopen.on();
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
    LedBlueon();
    LedRedoff();
    Buzzeroff();
    LINE.notify(String(name_device) + " DOOR CLOSE!!");
    Blynk.notify(String(name_device) + " DOOR CLOSE!!");
    Serial.println("@@CLOSE");
  }
  if (mcp.digitalRead(DoorGpioPin) == HIGH && DoorState == closed) { // เปิดประตู
    DoorState = opened;
    Door = 1;
    LedRedon();
    LedBlueoff();
    LINE.notify(String(name_device) + " DOOR OPEN!!");
    Blynk.notify(String(name_device) + " DOOR OPEN!!");
    Serial.println("@@OPEN");
  }
  //งัดประตู
  if (Door == 1 && Relay == 0) {
    Buzzeron();
    Relayclose();
    Serial.println("Attack the door!!");
    LINE.notify(String(name_device) + " Be attacked!!");
    Blynk.notify(String(name_device) + " Be attacked!!");
  }
  //เอาคีย์ไม่ทราบมาแตะ ประตูปิดอยู่ ประตูล๊อกอยู่ ให้แจ้งเตือน
  if (Keycard == 1 && Door == 0 && Relay == 0) {
    Buzzeron();
    Relayclose();
    Serial.println("Attack door by keycard!!");
    LINE.notify(String(name_device) + " Be attacked by keycard!!");
    Blynk.notify(String(name_device) + " Be attacked by keycard!!");
  }
  //เอาคีย์ไม่ทราบมาแตะ งัดประตู ประตูล๊อกอยู่ ให้แจ้งเตือน
  if (Keycard == 1 && Door == 1 && Relay == 0) {
    Buzzeron();
    Relayclose();
    Serial.println("Attack door by keycard case 2!!");
    LINE.notify(String(name_device) + " Be attacked by keycard!!");
    Blynk.notify(String(name_device) + " Be attacked by keycard!!");
  }
  // Normal
  //เอาคีย์การ์ดมาแตะ ประตูปิด ประตูล๊อก  ไม่ต้องแจ้งเตือน และปลดล๊อกประตู
  if (Keycard == 0 && Door == 0 && Relay == 0) {
    Buzzeroff();
    Relayopen();
    delay(5000);
    Relayclose();
    Keycard = 2;
  }
  
}
BLYNK_WRITE(V0) {
  ButtonStatelock = param.asInt();
  if (ButtonStatelock == 0) {
    Relayclose();
    LedBlueon();
    LedRedoff();
    Blynk.notify("LOCK THE DOOR");
  }
  if (ButtonStatelock == 1) {
    Relayopen();
    LedRedon();
    LedBlueoff();
    Blynk.notify("UNLOCK THE DOOR");
  }
}
BLYNK_WRITE(V4) {
  ButtonStatelocks = param.asInt();
  if (ButtonStatelocks == 0) {
    Relayclose();
    LedBlueon();
    LedRedoff();
    Blynk.notify("LOCK THE DOOR");
  }
  if (ButtonStatelocks == 1) {
    Relayopen();
    LedRedon();
    LedBlueoff();
    Blynk.notify("UNLOCK THE DOOR");
  }
}
void Relayclose() {
  Relay = 0;
  mcp.digitalWrite(RelayGpioPin, LOW);
}
void Relayopen() {
  Relay = 1;
  mcp.digitalWrite(RelayGpioPin, HIGH);
}
void Buzzeron() {
  mcp.digitalWrite(BuzzerGpioPin, LOW);
}
void Buzzeroff() {
  mcp.digitalWrite(BuzzerGpioPin, HIGH);
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
          Blynk.notify(String(name_device) + " Access by Admin!!");
          Serial.print("ADMIN!!");
        }
        else if (Text == "") {
          Keycard = 1;
          LINE.notify(String(name_device) + " Try to Access by Unknow!!");
          Blynk.notify(String(name_device) + " Try to Access by Unknow!!");
          Serial.print("UNKNOW!!");

        } else {
          Keycard = 1;
          LINE.notify(String(name_device) + " Try to Access by Unknow!!");
          Blynk.notify(String(name_device) + " Try to Access by Unknow!!");
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
  delay(1000);
}
