#include <EEPROM.h>
#include <Ticker.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TridentTD_LineNotify.h>
#define BLYNK_PRINT Serial
#define BLYNK_MAX_SENDBYTES 256
#define EEPROM_SALT 12663

#define CLOSED 0
#define OPENED 1
#define VT_PIN A0
#define AT_PIN A0
#define RELAYPIN D1
#define MAGNETICPIN D2
#define SWITCHPIN D0
#define LEDSTATUS D5

typedef struct
{
  int salt = EEPROM_SALT;
  char blynkToken[33] = "";
  char lineToken[45] = "";
  char namedevice[20] = "";
} WMSettings;
WMSettings settings;

bool BLYNK_ENABLED = true;
bool LINE_ENABLED = true;
bool shouldSaveConfig = false;
bool CHECKCLOSES = false;
bool CHECKOPENS = false;
bool STATUSDOOR = false;
bool STATUSRELAY = false;
bool STATUSSWITCH = false;
bool RESET = false;

float VOLTAGE, CURRENT;

int VT_TEMP, AT_TEMP;
int BT1, BT2, BTRESET;
int DOORSTATE = 0;

WiFiManager wifiManager;
Ticker ticker;

WidgetLED LED_RED_OPEN(V3);
WidgetLED LED_BLUE_CLOSE(V4);
WidgetLED LED_ALERT(V8);

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
  Blynk.syncVirtual(V6);
  Blynk.syncVirtual(V7);
  Blynk.syncVirtual(V8);
  Blynk.syncVirtual(V9);
  Blynk.syncAll();
}
BLYNK_WRITE(V0)
{
  BT1 = param.asInt();
  if (BT1 == 0)
  {
    CHECKCLOSES = true;
  }
  if (BT1 == 1)
  {
    CHECKOPENS = true;
  }
}
BLYNK_WRITE(V7)
{
  BT2 = param.asInt();
  if (BT2 == 0)
  {
    CHECKCLOSES = true;
  }
  if (BT2 == 1)
  {
    CHECKOPENS = true;
  }
}
BLYNK_WRITE(V9)
{
  BTRESET = param.asInt();
  if (BTRESET == 1)
  {
    RESET = true;
  }
}
void RELAYCLOSE()
{
  STATUSRELAY = false;
  digitalWrite(RELAYPIN, LOW);
}
void RELAYOPEN()
{
  STATUSRELAY = true;
  digitalWrite(RELAYPIN, HIGH);
}
void SWITCH()
{
  if (digitalRead(SWITCHPIN) == LOW)
  {
    STATUSSWITCH = false;
  }
  if (digitalRead(SWITCHPIN) == HIGH)
  {
    STATUSSWITCH = true;
  }
}
void DOOR()
{
  if (digitalRead(MAGNETICPIN) == HIGH && DOORSTATE == OPENED)
  {
    DOORSTATE = CLOSED;
    STATUSDOOR = false;
    Serial.println("CLOSE");
  }
  if (digitalRead(MAGNETICPIN) == LOW && DOORSTATE == CLOSED)
  {
    DOORSTATE = OPENED;
    STATUSDOOR = true;
    Serial.println("OPEN");
  }
}
void tick()
{
  int state = digitalRead(LEDSTATUS);
  digitalWrite(LEDSTATUS, !state);
}
void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
  Serial.println("  Enter Config Wifi ");
}
void resetFactory()
{
  wifiManager.resetSettings();
  WMSettings defaults;
  settings = defaults;
  EEPROM.begin(512);
  EEPROM.put(0, settings);
  EEPROM.end();
  Serial.println("ResetFactory");
  LINE.notify("รีเซ็ตค่าในอุปกรณ์เรียบร้อยแล้ว :)");
  delay(500);
  WiFi.disconnect();
  delay(500);
  ESP.reset();
  delay(500);
}

void setup()
{

  Serial.begin(115200);
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW);
  pinMode(SWITCHPIN, INPUT);
  digitalWrite(SWITCHPIN, LOW);
  pinMode(MAGNETICPIN, INPUT);

  LED_RED_OPEN.off();
  LED_BLUE_CLOSE.off();
  LED_ALERT.off();

  //wifiManager.resetSettings();

  pinMode(LEDSTATUS, OUTPUT);
  ticker.attach(0.6, tick);

  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setConfigPortalTimeout(180);

  EEPROM.begin(512);
  EEPROM.get(0, settings);
  EEPROM.end();

  if (settings.salt != EEPROM_SALT)
  {
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

  if (!wifiManager.autoConnect("SMARTLOCKALERT"))
  {
    Serial.println("failed to connect and hit timeout");
    delay(500);
    ESP.reset();
    delay(1000);
  }
  if (shouldSaveConfig)
  {
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
  if (strlen(settings.blynkToken) == 0)
  {
    BLYNK_ENABLED = false;
  }
  if (strlen(settings.lineToken) == 0)
  {
    LINE_ENABLED = false;
  }
  if (BLYNK_ENABLED)
  {
    Blynk.config(settings.blynkToken);
  }
  if (LINE_ENABLED)
  {
    LINE.setToken(settings.lineToken);
    LINE.notifySticker(String(settings.namedevice) + " LINE...CONNECTED SUCCESS :)", 2, 34);
  }
  Serial.println("CONNECTED SUCCESS :)");
  ticker.detach();
  digitalWrite(LEDSTATUS, LOW);
}
void loop()
{

  if (BLYNK_ENABLED)
  {
    Blynk.run();
  }
  if (!Blynk.connected())
  {
    Blynk.connect();
  }
  if (RESET)
  {
    resetFactory();
    RESET = false;
  }
  DOOR();
  SWITCH();
  Blynk.virtualWrite(V1, STATUSDOOR);  // MAGNETIC
  Blynk.virtualWrite(V2, STATUSRELAY); // RELAY
  VT_TEMP = analogRead(VT_PIN);
  VOLTAGE = VT_TEMP * (3.40 / 1023.0) * 5;
  AT_TEMP = analogRead(AT_PIN);
  CURRENT = AT_TEMP * (3.40 / 1023.0);
  Blynk.virtualWrite(V5, VOLTAGE);
  Blynk.virtualWrite(V6, CURRENT);

  if ((STATUSDOOR == false) && (STATUSRELAY == false))
  {
    LED_ALERT.off();
  }
  if ((STATUSDOOR == true) && (STATUSRELAY == false))
  {
    //BUZZERON();
    RELAYCLOSE();
    LED_ALERT.on();
    LED_BLUE_CLOSE.off();
    Blynk.virtualWrite(V0, STATUSRELAY);
    Blynk.virtualWrite(V1, STATUSDOOR);
    LINE.notifySticker(String(settings.namedevice) + " Attack the door!!", 2, 43);
    Serial.println("Attack the door!!");
  }
  if (BT1 == 0 && CHECKCLOSES == true)
  {
    RELAYCLOSE();
    LED_RED_OPEN.off();
    LED_BLUE_CLOSE.on();
    Blynk.virtualWrite(V0, STATUSRELAY);
    LINE.notifySticker(String(settings.namedevice) + " DOOR CLOSE!!", 2, 41);
    Serial.println("DOOR CLOSE");
    CHECKCLOSES = false;
  }
  if (BT1 == 1 && CHECKOPENS == true)
  {
    RELAYOPEN();
    LED_RED_OPEN.on();
    LED_BLUE_CLOSE.off();
    Blynk.virtualWrite(V0, STATUSRELAY);
    LINE.notifySticker(String(settings.namedevice) + " DOOR OPEN!!", 2, 40);
    Serial.println("DOOR OPEN");
    CHECKOPENS = false;
  }
  if (BT2 == 0 && CHECKCLOSES == true)
  {
    RELAYCLOSE();
    LED_RED_OPEN.off();
    LED_BLUE_CLOSE.on();
    Blynk.virtualWrite(V0, STATUSRELAY); // RELAY
    LINE.notifySticker(String(settings.namedevice) + " DOOR CLOSE!!", 2, 41);
    Serial.println("DOOR CLOSE");
    CHECKCLOSES = false;
  }
  if (BT2 == 1 && CHECKOPENS == true)
  {
    RELAYOPEN();
    LED_RED_OPEN.on();
    LED_BLUE_CLOSE.off();
    Blynk.virtualWrite(V0, STATUSRELAY); // RELAY
    LINE.notifySticker(String(settings.namedevice) + " DOOR OPEN!!", 2, 40);
    Serial.println("DOOR OPEN");
    CHECKOPENS = false;
  }
  if (STATUSSWITCH)
  {
    RELAYOPEN();
    LED_RED_OPEN.on();
    LED_BLUE_CLOSE.off();
    Blynk.virtualWrite(V0, STATUSRELAY); // RELAY
    LINE.notifySticker(String(settings.namedevice) + " DOOR OPEN!!", 2, 40);
    Serial.println("DOOR OPEN");
    delay(3000);
    RELAYCLOSE();
    LED_RED_OPEN.off();
    LED_BLUE_CLOSE.on();
    Blynk.virtualWrite(V0, STATUSRELAY); // RELAY
    LINE.notifySticker(String(settings.namedevice) + " DOOR CLOSE!!", 2, 41);
    Serial.println("DOOR CLOSE");
    CHECKOPENS = false;
    CHECKCLOSES = false;
  }
}
