#include <EEPROM.h>
#include <Ticker.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TridentTD_LineNotify.h>
#define BLYNK_PRINT Serial
#define EEPROM_SALT 12663

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

WiFiManager wifiManager;
Ticker ticker;
WidgetLED LEDCLOSE(V2);
WidgetLED LEDOPEN(V3);

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
  Blynk.syncAll();
}

BLYNK_WRITE(V0)
{
  STATUSRELAY = param.asInt();
  if (STATUSRELAY == 0)
  {
  }
  if (STATUSRELAY == 1)
  {
  }
}
BLYNK_WRITE(V1)
{
  BTRESET = param.asInt();
  if (BTRESET == 1)
  {
    Reset = true;
    resetFactory();
  }
}

void tick()
{
  int state = digitalRead(BUILTIN_LED);
  digitalWrite(BUILTIN_LED, !state);
}
void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
  Serial.println("  Enter Config Wifi ");
}
void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void resetFactory()
{
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
void setup()
{

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

  if (!wifiManager.autoConnect("SMART LOCK ALERT"))
  {
    Serial.println("failed to connect and hit timeout");
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
  if (BLYNK_ENABLED)
  {
    Blynk.config(settings.blynkToken);
  }
  if (strlen(settings.lineToken) == 0)
  {
    LINE_ENABLED = false;
  }
  if (LINE_ENABLED)
  {
    LINE.setToken(settings.lineToken);
  }
  Serial.println("connected...Success :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);

  if (LINE_ENABLED)
  {
    LINE.notify(String(settings.namedevice) + " Connect Finish!!");
    LINE.notify("!! Online !!");
    LINE.notify("Start..");
  }
  if (BLYNK_ENABLED)
  {
    Blynk.notify(String(settings.namedevice) + " Connect Finish !!");
    Blynk.notify("!! Online !!");
    Blynk.notify("Start..");
  }
}
void loop()
{

  if (BLYNK_ENABLED)
  {
    Blynk.run();
  }
}
