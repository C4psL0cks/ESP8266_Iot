#include <EEPROM.h>
#include <Ticker.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#define EEPROM_SALT 12663

typedef struct {
  int salt = EEPROM_SALT;
  char blynkToken[33] = "";
  char lineToken[45] = "";
  char namedevice[20] = "";
} WMSettings;
WMSettings settings;

bool shouldSaveConfig = false;

WiFiManager wifiManager;
Ticker ticker;

void tick() {
  int state = digitalRead(BUILTIN_LED);
  digitalWrite(BUILTIN_LED, !state);
}

void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
  Serial.println("  Enter Config Wifi ");
}

void saveConfigCallback() {
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

void setup() {

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

  if (!wifiManager.autoConnect("XXX","123456")) {
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


  Serial.println("connected...Success :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);
  Serial.println(String(settings.namedevice) + " Connect Finish!!");
  Serial.println("!! Online !!");

}

void loop() {

  if (Serial.available() > 0) {
    int number = Serial.parseInt();
    Serial.println("Select :" + String(number));
    if (number == 1) {
      resetFactory();
      Serial.println("Reset");
    }
  }
}
