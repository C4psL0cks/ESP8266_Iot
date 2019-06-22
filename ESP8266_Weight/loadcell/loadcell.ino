#include <EEPROM.h>
#include <Ticker.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "HX711.h"
#define EEPROM_SALT 12663
#define calibration_factor -20189
#define DOUT  D2
#define SCK  D3
#define LEDSTATUS   D6
#define SWITCHPIN   D10

HX711 scale(DOUT, SCK);

float maxweight = 0;
float weight;
float checks;
int count = 0;

bool shouldSaveConfig = false;
bool FIREBASE_ENABLED = true;
bool RESET = false;

typedef struct {
  int   salt = EEPROM_SALT;
  char  FIREBASE_HOST[50]  = "";
  char  FIREBASE_AUTH[40] = "";
} WMSettings;
WMSettings settings;

WiFiManager wifiManager;
Ticker ticker;

void SWITCH() {
  if (digitalRead(SWITCHPIN) == LOW) {
    RESET = false;
  }
  if (digitalRead(SWITCHPIN) == HIGH) {
    RESET = true;
  }
}
void tick() {
  int state = digitalRead(LEDSTATUS);
  digitalWrite(LEDSTATUS, !state);
}
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void configModeCallback (WiFiManager * myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
  Serial.println("  Enter Config Wifi ");
}
void resetFactory() {
  wifiManager.resetSettings();
  WMSettings defaults;
  settings = defaults;
  EEPROM.begin(512);
  EEPROM.put(0, settings);
  EEPROM.end();
  Serial.println("ResetFactory");
  delay(500);
  WiFi.disconnect();
  delay(500);
  ESP.reset();
  delay(500);
}
void setup(void) {
  Serial.begin(115200);
  scale.set_scale(calibration_factor);
  scale.tare();

  pinMode(SWITCHPIN, INPUT);    digitalWrite(SWITCHPIN, LOW);
  pinMode(LEDSTATUS, OUTPUT);
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

  Serial.println(settings.FIREBASE_HOST);
  Serial.println(settings.FIREBASE_AUTH);
  WiFiManagerParameter custom_FIREBASE_HOST("FIREBASE_HOST", "FIREBASE_HOST", settings.FIREBASE_HOST, 50);
  wifiManager.addParameter(&custom_FIREBASE_HOST);
  WiFiManagerParameter custom_FIREBASE_AUTH("FIREBASE_AUTH", "FIREBASE_AUTH", settings.FIREBASE_AUTH, 40);
  wifiManager.addParameter(&custom_FIREBASE_AUTH);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  if (!wifiManager.autoConnect("THE WEIGHT")) {
    Serial.println("failed to connect and hit timeout");
    delay(500);
    ESP.reset();
    delay(1000);
  }
  if (shouldSaveConfig) {
    Serial.println("Saving config");
    strcpy(settings.FIREBASE_HOST, custom_FIREBASE_HOST.getValue());
    strcpy(settings.FIREBASE_AUTH, custom_FIREBASE_AUTH.getValue());
    Serial.println(settings.FIREBASE_HOST);
    Serial.println(settings.FIREBASE_AUTH);
    EEPROM.begin(512);
    EEPROM.put(0, settings);
    EEPROM.end();
  }
  if (strlen(settings.FIREBASE_HOST) == 0) {
    FIREBASE_ENABLED = false;
  }
  if (FIREBASE_ENABLED) {
    Firebase.begin(settings.FIREBASE_HOST, settings.FIREBASE_AUTH);
  }
  Serial.println("CONNECTED SUCCESS :)");
  ticker.detach();
  digitalWrite(LEDSTATUS, LOW);
}
void loop(void) {
  
  SWITCH();
  
  if(RESET){
    resetFactory();
    RESET = false;
  }

  weight = scale.get_units();
  if (weight <= 2) {
    weight = 0.0;
  }
  if (weight != 0) {
    Serial.println("***************************");
    Serial.println("Weight Default :" + String(weight));
    checks = weight;
    Serial.println("Checks Default :" + String(checks));
    if (checks == weight) {
      Serial.println("Weight Balance");
      count += 1;
      Serial.println("Weight Balance:" + String(weight));
      Serial.println("Checks  Balance:" + String(checks));
      Serial.println("***************************");
      delay(500);
    }
    if (count == 10) {
      Serial.println("======== Send =========");
      maxweight = weight;
      Serial.println("Weight Send:" + String(weight));
      Serial.println("Max weight :" + String(maxweight));
      // update value
      Firebase.setFloat("weight", maxweight);
      Firebase.setFloat("user/weighing/", maxweight);
      // handle error
      if (Firebase.failed()) {
        Serial.print("setting /number failed:");
        Serial.println(Firebase.error());
        return;
      }
      delay(500);
    }
  }
  if (count == 10) {
    Serial.println("Reset");
    delay(2000);
    // remove value
    Firebase.remove("weight");
    delay(1000);
    count = 0;
  }
}
