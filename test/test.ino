#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <TridentTD_LineNotify.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial
#define BLYNK_MAX_SENDBYTES 256
#define closed 0
#define opened 1

#include <Wire.h>
#include "Adafruit_MCP23017.h"
Adafruit_MCP23017 mcp;

#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
PN532_SPI pn532spi(SPI, 16);
PN532 nfc(pn532spi);
uint8_t currentblock = 1; // block write select
uint8_t data[20];         // block write data

#define VT_PIN A0 // connect VT
#define AT_PIN A0// connect AT
#define ARDUINO_WORK_VOLTAGE 3.3

WiFiManager wifiManager;

WidgetLED Ledopen(V1);
WidgetLED Ledclose(V2);
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
}
bool shouldSaveConfig = false;
const int LedGpioPinRed = 8;
const int LedGpioPinBlue = 9;
const int RelayGpioPin = 10;
const int DoorGpioPin = 11;
const int BuzzerGpioPin = 12;
const int DoorUnLockGpioPin = 13;
int ButtonStatelock;
int DoorState = 0;   //Doorstate
int Door = 0;                         // Start 0
int Relay = 0;                       // Start 0
int Keycard = 2;                      // Start 2
int App = 2;

char blynk_token[35] = "";
char line_token[45] = "";
char name_device[35] = "";
char ssid[20] = "";
char pass[20] = "";

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void setup() {

  Serial.begin(115200);
  mcp.begin();
  mcp.pinMode(LedGpioPinRed, OUTPUT);
  mcp.pinMode(LedGpioPinBlue, OUTPUT);
  mcp.pinMode(RelayGpioPin, OUTPUT);
  mcp.pinMode(BuzzerGpioPin, OUTPUT);
  mcp.pinMode(DoorGpioPin, INPUT);
  mcp.pullUp(DoorGpioPin, HIGH);
  mcp.pinMode(DoorUnLockGpioPin, INPUT);
  mcp.pullUp(DoorUnLockGpioPin, HIGH);

  //clean FS, for testing
  SPIFFS.format();

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

  //reset settings - for testing
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
BLYNK_WRITE(V0) {
  ButtonStatelock = param.asInt();
  if (ButtonStatelock == 1) { // close
    Relayclose(); // ative low
    Door = 0;
    Ledopen.on();
    Ledclose.off();
    LINE.notify(String(name_device) + " CLOSE");
  }
  if (ButtonStatelock == 0) { // open
    Relayopen(); // non-active high
    Door = 1;
    Ledclose.on();
    Ledopen.off();
    LINE.notify(String(name_device) + " OPEN");
  }
}
BLYNK_WRITE(V5) { // timer
  ButtonStatelock = param.asInt();
  if (ButtonStatelock == 1) { // close
    Relayclose(); // ative low
    Door = 0;
    Ledopen.on();
    Ledclose.off();
    LINE.notify(String(name_device) + " CLOSE");
  }
  if (ButtonStatelock == 0) { // open
    Relayopen(); // non-active high
    Door = 1;
    Ledclose.on();
    Ledopen.off();
    LINE.notify(String(name_device) + " OPEN");
  }
}
void loop() {

  Blynk.run();
  int vt_temp = analogRead(VT_PIN);
  int at_temp = analogRead(AT_PIN);
  double voltage = vt_temp * (ARDUINO_WORK_VOLTAGE / 1023.0) * 5;
  double current = at_temp * (ARDUINO_WORK_VOLTAGE / 1023.0);
  Blynk.virtualWrite(3, voltage);
  Blynk.virtualWrite(4, current);
  Blynk.virtualWrite(5, Door);
  Blynk.virtualWrite(6, Relay);


  RFID();

  if (mcp.digitalRead(DoorGpioPin) == HIGH && DoorState == opened) {
    DoorState = closed;
    Door = 0;
    //Buzzeroff();
    //LedBlueon();
    //LedRedoff();
    Serial.println("CLOSE");
    //LINE.notify("Chanal" + String(name_device) + "DoorClose");
  }
  if (mcp.digitalRead(DoorGpioPin) == LOW && DoorState == closed) {
    DoorState = opened;
    Door = 1;
    //Buzzeron();
    //LedRedon();
    //LedBlueoff();
    Serial.println("OPEN");
    //LINE.notify("Chanal" + String(name_device) + "DoorClose");
  }
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
void Relayopen() {
  Relay = 1;
  mcp.digitalWrite(RelayGpioPin, HIGH);
}
void Relayclose() {
  Relay = 0;
  mcp.digitalWrite(RelayGpioPin, LOW);
}
void LedRedon() {
  mcp.digitalWrite(LedGpioPinRed, HIGH);
}
void LedRedoff() {
  mcp.digitalWrite(LedGpioPinRed, LOW);
}
void LedBlueon() {
  mcp.digitalWrite(LedGpioPinBlue, HIGH);
}
void LedBlueoff() {
  mcp.digitalWrite(LedGpioPinBlue, LOW);
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
