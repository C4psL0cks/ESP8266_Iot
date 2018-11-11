#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

//#include <TridentTD_LineNotify.h>
#include <FirebaseArduino.h>
#define FIREBASE_HOST "espdemo-6518c.firebaseio.com"
#define FIREBASE_AUTH "YhIZkV7dc8UGALQFalhwxRjqbihIDCM9SVcQoN2s"

#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
PN532_SPI pn532spi(SPI, 16);
PN532 nfc(pn532spi);

uint8_t currentblock = 1;             // block write select
uint8_t data[20];                     // block write data
uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uidLength;
bool Keycard = false;
bool state = false;

WiFiManager wifiManager;
bool shouldSaveConfig = false;
char token[35] = "";
char line_token[45] = "";
char name_device[35] = "";

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void setup() {
  Serial.begin(115200);
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
          strcpy(token, json["token"]);
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
  WiFiManagerParameter custom_token("token", "Token", token, 35);
  WiFiManagerParameter custom_line_token("line", "line token", line_token, 45);
  WiFiManagerParameter custom_name_device("name", "name device", name_device, 35);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_token);
  wifiManager.addParameter(&custom_line_token);
  wifiManager.addParameter(&custom_name_device);

  wifiManager.resetSettings();

  if (!wifiManager.autoConnect("SMART LOCK ALERT")) {
  }
  Serial.println("connected...success :)");
  strcpy(token, custom_token.getValue());
  strcpy(line_token, custom_line_token.getValue());
  strcpy(name_device, custom_name_device.getValue());

  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    json["token"] = token;
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
  //LINE.setToken(line_token);

  Serial.printf(" ESP8266 Chip id = %08X\n", ESP.getChipId());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  nfc.begin();
  nfc.SAMConfig();
  Serial.println("Start Read...");
}
void loop() {

  if (Firebase.getString("/user/token") == token) {
    state = true;
  }
  if (state == true) {
    RFID();
    Serial.println("OTP OK");
  }
  if (Firebase.failed()) {
    Serial.print("GET / failed:");
    Serial.println(Firebase.error());
    return;
  }
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
