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
  mcp.pinMode(LedGpioPinBlue, OUTPUT);
  mcp.pinMode(RelayGpioPin, OUTPUT);
  mcp.pinMode(BuzzerGpioPin, OUTPUT);
  mcp.pinMode(DoorGpioPin, INPUT);
  mcp.pullUp(DoorGpioPin, HIGH);
  mcp.pinMode(DoorUnLockGpioPin, INPUT);
  mcp.pullUp(DoorUnLockGpioPin, HIGH);

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
  Blynk.virtualWrite(V3, voltage); // volt
  Blynk.virtualWrite(V5, Door);   // door status
  Blynk.virtualWrite(V6, Relay);  // lock status

  RFID();

  if (mcp.digitalRead(DoorGpioPin) == LOW && DoorState == opened) { // ปิดประตู
    DoorState = closed;
    Door = 0;
    Buzzeroff();
    LedBlueon();
    LedRedoff();
    LINE.notify(String(name_device) + "-->DOORCLOSE!!");
    Serial.println("CLOSE");
  }
  if (mcp.digitalRead(DoorGpioPin) == HIGH && DoorState == closed) { // เปิดประตู
    DoorState = opened;
    Door = 1;
    Buzzeron();
    LedRedon();
    LedBlueoff();
    LINE.notify(String(name_device) + "-->DOOROPEN!!");
    Serial.println("OPEN");
  }
}
//// Normal
BLYNK_WRITE(V0) {
  ButtonStatelock = param.asInt();
  if (ButtonStatelock == 1) {
    Relayclose();
    Door = 0;
    LedRedon();
    LedBlueoff();
    LINE.notify(String(name_device) + "-->DOORCLOSE!!");
  }
  if (ButtonStatelock == 0) {
    Relayopen();
    Door = 1;
    LedBlueon();
    LedRedoff();
    LINE.notify(String(name_device) + "-->DOOROPEN!!");
  }
}
//// timer
BLYNK_WRITE(V4) {
  ButtonStatelock = param.asInt();
  if (ButtonStatelock == 1) {
    Relayclose();
    Door = 0;
    LedRedon();
    LedBlueoff();
    LINE.notify(String(name_device) + " CLOSE");
  }
  if (ButtonStatelock == 0) {
    Relayopen();
    Door = 1;
    LedBlueon();
    LedRedoff();
    LINE.notify(String(name_device) + " OPEN");
  }
}
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
  mcp.digitalWrite(LedGpioPinRed, HIGH);
}
void LedRedoff() {
  Ledopen.off();
  mcp.digitalWrite(LedGpioPinRed, LOW);
}
void LedBlueon() {
  Ledclose.on();
  mcp.digitalWrite(LedGpioPinBlue, HIGH);
}
void LedBlueoff() {
  Ledclose.off();
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
          Keycard = 1;
          LINE.notify(String(name_device) + " Access by Admin!!");
          Serial.print("ADMIN!!");
        }
        else if (Text == "") {
          Keycard = 0;
          LINE.notify(String(name_device) + " Access by Unknow!!");
          Serial.print("UNKNOW!!");

        } else {
          Keycard = 0;
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
