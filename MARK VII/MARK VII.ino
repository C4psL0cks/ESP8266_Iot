#include <EEPROM.h>
#include <Ticker.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
#include <TridentTD_LineNotify.h>
#define BLYNK_PRINT Serial
#define EEPROM_SALT 12663

#define VT_PIN      A0
#define CLOSED      0
#define OPENED      1
#define LEDPINRED   8
#define LEDPINBLUE  9
#define RELAYPIN    10
#define MAGNETICPIN 11
#define BUZZERPIN   12
#define SWITCHPIN   13

int VT_TEMP = 0;
int VOLTAGE = 0;
int DOORSTATE = 0;

int STATUSKEYCARD = 2;
int STATUSDOOR = 0;
int STATUSRELAY = 0;
int STATUSSWITCH = 0;

int BTRESET;
int BTTIMER;

typedef struct {
  int   salt = EEPROM_SALT;
  char  blynkToken[33]  = "";
  char  lineToken[45] = "";
  char  namedevice[20] = "";
} WMSettings;
WMSettings settings;

uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t uidLength;
uint8_t currentblock = 1;
uint8_t data[20];

bool BLYNK_ENABLED = true;
bool LINE_ENABLED = true;
bool shouldSaveConfig = false;
bool case1 = false;
bool case2 = false;
bool case3 = false;
bool case4 = false;
bool case5 = false;
bool case6 = false;
bool Reset = false;

WiFiManager wifiManager;
Ticker ticker;
WidgetLED LEDCLOSE(V2);
WidgetLED LEDOPEN(V3);
PN532_SPI pn532spi(SPI, 16);
PN532 nfc(pn532spi);
Adafruit_MCP23017 mcp;

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
  Blynk.syncVirtual(V6);
  Blynk.syncVirtual(V7);
  Blynk.syncAll();
}

BLYNK_WRITE(V0) {
  STATUSRELAY = param.asInt();
  if (STATUSRELAY == 0) {
    RELAYCLOSE();
  }
  if (STATUSRELAY == 1) {
    RELAYOPEN();
  }
}
BLYNK_WRITE(V1) {
  BTRESET = param.asInt();
  if (BTRESET == 1) {
    Reset = true;
    resetFactory();
  }
}
BLYNK_WRITE(V5) {
  BTTIMER = param.asInt();
  if (BTTIMER == 0) {
    RELAYCLOSE();
  }
  if (BTTIMER == 1) {
    RELAYOPEN();
  }
}
void tick() {
  int state = digitalRead(BUILTIN_LED);
  digitalWrite(BUILTIN_LED, !state);
}
void configModeCallback (WiFiManager * myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
  Serial.println("  Enter Config Wifi ");

}
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void resetFactory() {
  WMSettings defaults;
  settings = defaults;
  EEPROM.begin(512);
  EEPROM.put(0, settings);
  EEPROM.end();
  wifiManager.resetSettings();
  delay(500);
  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.persistent(true);
  delay(500);
  WiFi.disconnect();
  delay(500);
  ESP.reset();
  delay(500);
}
void BUZZERON() {
  mcp.digitalWrite(BUZZERPIN, LOW);
}
void BUZZEROFF() {
  mcp.digitalWrite(BUZZERPIN, HIGH);
}
void LEDREDON() {
  LEDOPEN.on();
  LEDCLOSE.off();
  mcp.digitalWrite(LEDPINRED, HIGH);
}
void LEDREDOFF() {
  LEDOPEN.off();
  LEDCLOSE.on();
  mcp.digitalWrite(LEDPINRED, LOW);
}
void LEDBLUEON() {
  LEDCLOSE.on();
  LEDOPEN.off();
  mcp.digitalWrite(LEDPINBLUE, HIGH);
}
void LEDBLUEOFF() {
  LEDCLOSE.off();
  LEDOPEN.on();
  mcp.digitalWrite(LEDPINBLUE, LOW);
}
void RELAYCLOSE() {
  STATUSRELAY = 0;
  mcp.digitalWrite(RELAYPIN, LOW);
}
void RELAYOPEN() {
  STATUSRELAY = 1;
  mcp.digitalWrite(RELAYPIN, HIGH);
}
void SWITCH() {
  if (mcp.digitalRead(SWITCHPIN) == HIGH) {
    STATUSSWITCH = 1;
  }
  if (mcp.digitalRead(SWITCHPIN) == LOW) {
    STATUSSWITCH = 0;
  }
}
void DOOR() {
  if (mcp.digitalRead(MAGNETICPIN) == LOW && DOORSTATE == OPENED) {
    DOORSTATE = CLOSED;
    STATUSDOOR = 0;
    RELAYCLOSE();
    LEDBLUEON();
    LEDREDOFF();
    BUZZEROFF();
    Serial.println("CLOSE");
    case1 = true;
  }
  if (mcp.digitalRead(MAGNETICPIN) == HIGH && DOORSTATE == CLOSED) {
    DOORSTATE = OPENED;
    STATUSDOOR = 1;
    LEDBLUEOFF();
    LEDREDON();
    Serial.println("OPEN");
    case2 = true;
  }
}
void RFID() {
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    if (uidLength == 4) {
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, currentblock, 1, keya);
      if (success) {
        success = nfc.mifareclassic_ReadDataBlock(currentblock, data);
        if (success) {
          Serial.println();
          char check[20]; String Text; int num;
          for (int i = 0 ; i < sizeof(data); i++ ) {
            String text = String(data[i], DEC);
            num = text.toInt(); check[i] = num; Text += check[i];
          }
          if (Text == "ADMIN") {
            Serial.print("ADMIN!!");
            STATUSKEYCARD = 0;
          }
          else {
            Serial.print("UNKNOW!!");
            STATUSKEYCARD = 1;
          }
        }
      }
    }
  }
  else {
    Serial.println();
    Serial.println("Wait New Read!!");
  }
  delay(700);
}
void setup() {

  mcp.begin();
  mcp.pinMode(LEDPINRED, OUTPUT);   mcp.digitalWrite(LEDPINRED, HIGH);
  mcp.pinMode(LEDPINBLUE, OUTPUT);  mcp.digitalWrite(LEDPINBLUE, HIGH);
  mcp.pinMode(RELAYPIN, OUTPUT);    mcp.digitalWrite(RELAYPIN, LOW);
  mcp.pinMode(BUZZERPIN, OUTPUT);   mcp.digitalWrite(BUZZERPIN, HIGH);
  mcp.pinMode(MAGNETICPIN, INPUT);  mcp.pullUp(MAGNETICPIN, HIGH);
  mcp.pinMode(SWITCHPIN, INPUT);
  RELAYCLOSE();
  LEDBLUEON();
  LEDREDON();

  Serial.begin(115200);
  //reset saved settings
  //wifiManager.resetSettings();

  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.6, tick);

  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setConfigPortalTimeout(180);

  EEPROM.begin(512);
  EEPROM.get(0, settings);
  EEPROM.end();

  if (settings.salt != EEPROM_SALT) {
    Serial.println("Invalid settings in EEPROM, trying with defaults");
    WMSettings defaults;
    settings = defaults;
  }

  Serial.println(settings.blynkToken);
  Serial.println(settings.lineToken);
  Serial.println(settings.namedevice);

  WiFiManagerParameter custom_blynk_token("blynk-token", "blynk token", settings.blynkToken, 33);
  wifiManager.addParameter(&custom_blynk_token);
  WiFiManagerParameter custom_line_token("line-token", "line token", settings.lineToken, 45);
  wifiManager.addParameter(&custom_line_token);
  WiFiManagerParameter custom_name_device("name-device", "name device", settings.namedevice, 20);
  wifiManager.addParameter(&custom_name_device);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  if (!wifiManager.autoConnect("SMART LOCK ALERT")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }
  if (shouldSaveConfig) {
    Serial.println("Saving config");
    strcpy(settings.blynkToken, custom_blynk_token.getValue());
    strcpy(settings.lineToken, custom_line_token.getValue());
    strcpy(settings.namedevice, custom_name_device.getValue());
    Serial.println(settings.blynkToken);
    Serial.println(settings.lineToken);
    Serial.println(settings.namedevice);
    EEPROM.begin(512);
    EEPROM.put(0, settings);
    EEPROM.end();
  }
  if (strlen(settings.blynkToken) == 0) {
    BLYNK_ENABLED = false;
  }
  if (BLYNK_ENABLED) {
    Blynk.config(settings.blynkToken);
  }
  if (strlen(settings.lineToken) == 0) {
    LINE_ENABLED = false;
  }
  if (LINE_ENABLED) {
    LINE.setToken(settings.lineToken);
  }
  Serial.println("connected...Success :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);

  if (LINE_ENABLED) {
    LINE.notify(String(settings.namedevice) + " Connect Finish!!");
    LINE.notify("!! Online !!");
    LINE.notify("Start..");
  }
  if (BLYNK_ENABLED) {
    Blynk.notify(String(settings.namedevice) + " Connect Finish !!");
    Blynk.notify("!! Online !!");
    Blynk.notify("Start..");
  }

  nfc.begin();
  nfc.SAMConfig();
  Serial.println("Start Read...");
}
void loop() {

  if (BLYNK_ENABLED) {
    Blynk.run();
  }
  Blynk.virtualWrite(V0, STATUSRELAY);
  Blynk.virtualWrite(V5, BTTIMER);
  Blynk.virtualWrite(V6, STATUSDOOR);
  Blynk.virtualWrite(V7, STATUSRELAY);

  RFID();
  DOOR();
  SWITCH();

  VT_TEMP = analogRead(VT_PIN);
  VOLTAGE = VT_TEMP * (3.3 / 1023.0) * 5;
  Blynk.virtualWrite(V4, VOLTAGE);

  if ((STATUSDOOR == 1) && (STATUSRELAY == 0)) {
    BUZZERON();
    RELAYCLOSE();
    case3 = true;
    Serial.println("Attack the door!!");
  }
  if ((STATUSKEYCARD == 1) && (STATUSDOOR == 0) && (STATUSRELAY == 0)) {
    BUZZERON();
    RELAYCLOSE();
    case4 = true;
    Serial.println("Attack door by keycard!!");
  }
  if ((STATUSKEYCARD == 1) && (STATUSDOOR == 1) && (STATUSRELAY == 0)) {
    BUZZERON();
    RELAYCLOSE();
    case5 = true;
    Serial.println("Attack door by keycard!! AND Attack the door!!");
  }
  if ((STATUSKEYCARD == 0) && (STATUSDOOR == 0) && (STATUSRELAY == 0)) {
    BUZZEROFF();
    RELAYOPEN();
    delay(1000);
    RELAYCLOSE();
    STATUSKEYCARD = 2;
    case6 = true;
    Serial.println("Access by keycard!!");
  }
  if ((STATUSSWITCH == 1) && (STATUSDOOR == 0) && (STATUSRELAY == 0)) {
    BUZZEROFF();
    RELAYOPEN();
    delay(1000);
    RELAYCLOSE();
    STATUSSWITCH = 0;
    Serial.println("Access by switch!!");
  }

  if (LINE_ENABLED == true && case1 == true) {
    LINE.notify(String(settings.namedevice) + " DOOR CLOSR");
    case1 = false;
  }
  if (BLYNK_ENABLED == true && case1 == true) {
    Blynk.notify(String(settings.namedevice) + " DOOR CLOSE");
    case1 = false;
  }
  if (LINE_ENABLED == true && case2 == true) {
    LINE.notify(String(settings.namedevice) + " DOOR OPEN");
    case2 = false;
  }
  if (BLYNK_ENABLED == true && case2 == true) {
    Blynk.notify(String(settings.namedevice) + " DOOR OPEN");
    case2 = false;
  }
  if (LINE_ENABLED == true && case3 == true) {
    LINE.notify(String(settings.namedevice) + " Attack the door!!");
    case3 = false;
  }
  if (BLYNK_ENABLED == true && case3 == true) {
    Blynk.notify(String(settings.namedevice) + " Attack the door!!");
    case3 = false;
  }
  if (LINE_ENABLED == true && case4 == true) {
    LINE.notify(String(settings.namedevice) + " Attack door by keycard!!");
    case4 = false;
  }
  if (BLYNK_ENABLED == true && case4 == true) {
    Blynk.notify(String(settings.namedevice) + " Attack door by keycard!!");
    case4 = false;
  }
  if (LINE_ENABLED == true && case5 == true) {
    LINE.notify(String(settings.namedevice) + " Attack door by keycard!! AND Attack the door!!");
    case5 = false;
  }
  if (BLYNK_ENABLED == true && case5 == true) {
    Blynk.notify(String(settings.namedevice) + " Attack door by keycard!! AND Attack the door!!");
    case5 = false;
  }
  if (LINE_ENABLED == true && case6 == true) {
    LINE.notify(String(settings.namedevice) + " Access by keycard!!");
    case6 = false;
  }
  if (BLYNK_ENABLED == true && case6 == true) {
    Blynk.notify(String(settings.namedevice) + " Access by keycard!!");
    case6 = false;
  }

  if (LINE_ENABLED == true && Reset == true) {
    LINE.notify(String(settings.namedevice) + " RESET WIFI finish!!");
    Reset = false;
  }
  if (BLYNK_ENABLED == true && Reset == true) {
    Blynk.notify(String(settings.namedevice) + " RESET WIFI finish!!");
    Reset = false;
  }
}
