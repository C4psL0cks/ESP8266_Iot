#define BLYNK_PRINT Serial

#include "DHT.h"
#include <Wire.h> 
#include <EEPROM.h>
#include <Ticker.h>
#include <ArduinoOTA.h>  
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp8266.h>

static bool BLYNK_ENABLED = true;

#define DHTPIN     D5
#define fanOutPIN  D7
#define HeaterPIN  D6
#define statLED    16
#define buttonPIN  0

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,20,4);
BlynkTimer timer;

#include <TimeLib.h>
#include <WidgetRTC.h>

float TempLOW = 0.0;
float TempHIGH = 0.0;
float HumiLOW = 0.0;
float HumiHIGH = 0.0;
float Temp = 0.0;
float Humi = 0.0;

unsigned long startPress = 0;

#define EEPROM_SALT 12663

#define AUTO     0
#define MANUAL   1

typedef struct {
  int   salt = EEPROM_SALT;
  char  blynkToken[33]  = "";
  char  blynkServer[33] = "blynk-cloud.com";
  char  blynkPort[6]    = "8080";
} WMSettings;

WMSettings settings;

WidgetLED ledHeater(V9);
WidgetLED ledFanOut(V10);
WidgetLED ledLink(V11);

WidgetRTC rtc;
Ticker ticker;
//WiFiManager wifiManager;

boolean modeChT = AUTO;

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);

  lcd.setCursor(0,0);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print("  Enter Config Wifi ");
  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(0,3);
  lcd.print("                    ");
}

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void restart() {
  ESP.reset();
  delay(1000);
}

void resetFactory() {
  //reset settings to defaults
    WMSettings defaults;
    settings = defaults;
    EEPROM.begin(512);
    EEPROM.put(0, settings);
    EEPROM.end();

  delay(5000);
  //reset wifi credentials
  WiFi.persistent(false);      
  WiFi.disconnect(true);         
  WiFi.persistent(true);
  
  delay(5000);
  //WiFi.disconnect();
  //delay(1000);
  ESP.reset();
  //delay(1000);
}

void tick()
{
  //toggle state
  int state = digitalRead(statLED);  // get the current state of GPIO1 pin
  digitalWrite(statLED, !state);     // set pin to the opposite state
}

BLYNK_CONNECTED() {
  rtc.begin();
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
  Blynk.syncVirtual(V10);
  Blynk.syncVirtual(V11);
  Blynk.syncVirtual(V12);
  Blynk.syncAll();
}

BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(pinValue != 1)
  {
    modeChT = AUTO;
    ledHeater.off();
    ledFanOut.off();
    digitalWrite(HeaterPIN, HIGH);
    digitalWrite(fanOutPIN, HIGH);
    Blynk.virtualWrite(V7, "Inactive");
    Blynk.setProperty(V7, "color", "#3D3D3D");
    Blynk.virtualWrite(V8, "Inactive");
    Blynk.setProperty(V8, "color", "#3D3D3D");
  }else
  {
    modeChT = MANUAL;
    ledHeater.off();
    ledFanOut.off();
    digitalWrite(HeaterPIN, HIGH);
    digitalWrite(fanOutPIN, HIGH);
    Blynk.virtualWrite(V7, LOW);
    Blynk.setProperty(V7, "color", "#000000");
    Blynk.virtualWrite(V8, "Inactive");
    Blynk.setProperty(V8, "color", "#3D3D3D");
  }
}

BLYNK_WRITE(V7)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(modeChT == MANUAL)
  {
    if(pinValue == 1)
    {
      ledHeater.on();
      digitalWrite(HeaterPIN, LOW);
    }else
    {
      ledHeater.off();
      digitalWrite(HeaterPIN, HIGH);
    }
  }
  else
  {
    Blynk.virtualWrite(V7, HIGH);
    Blynk.virtualWrite(V7, "Inactive");
    Blynk.setProperty(V7, "color", "#3D3D3D");
  }
}

BLYNK_WRITE(V8)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(modeChT == MANUAL)
  {
    if(pinValue == 1)
    {
      ledFanOut.on();
      digitalWrite(fanOutPIN, LOW);
    }else
    {
      ledFanOut.off();
      digitalWrite(fanOutPIN, HIGH);
    }
  }
  else
  {
    Blynk.virtualWrite(V8, HIGH);
    Blynk.virtualWrite(V8, "Inactive");
    Blynk.setProperty(V8, "color", "#3D3D3D");
  }
}

BLYNK_WRITE(V3)
{
  TempLOW = param.asFloat(); // assigning incoming value from pin V1 to a variable
  EEPROM.begin(512);
  EEPROM.put(100, TempLOW);
  EEPROM.put(110, TempHIGH);
  EEPROM.end();
}

BLYNK_WRITE(V4)
{
  TempHIGH = param.asFloat(); // assigning incoming value from pin V1 to a variable
  EEPROM.begin(512);
  EEPROM.put(100, TempLOW);
  EEPROM.put(110, TempHIGH);
  EEPROM.end();
}

BLYNK_WRITE(V5)
{
  HumiLOW = param.asFloat(); // assigning incoming value from pin V1 to a variable
  EEPROM.begin(512);
  EEPROM.put(120, HumiLOW);
  EEPROM.put(130, HumiHIGH);
  EEPROM.end();
}

BLYNK_WRITE(V6)
{
  HumiHIGH = param.asFloat(); // assigning incoming value from pin V1 to a variable
  EEPROM.begin(512);
  EEPROM.put(120, HumiLOW);
  EEPROM.put(130, HumiHIGH);
  EEPROM.end();
}

void blinkLedWidget()
{
  if (ledLink.getValue()) {
    ledLink.off();
  } else {
    ledLink.on();
  }

  Blynk.virtualWrite(V12, WiFi.RSSI());
}

void dhtSend()
{
  Humi = dht.readHumidity();
  Temp = dht.readTemperature();
  Blynk.virtualWrite(V0, Temp);
  Blynk.virtualWrite(V1, Humi);
}

void reconnectBlynk() 
{
  if (!Blynk.connected()) {
    if (Blynk.connect()) {
      BLYNK_LOG("Reconnected");
    } else {
      BLYNK_LOG("Not reconnected");
    }
  }
}

void setup() 
{
  pinMode(statLED, OUTPUT);      digitalWrite(statLED, HIGH);
  pinMode(HeaterPIN, OUTPUT);    digitalWrite(HeaterPIN, HIGH);
  pinMode(fanOutPIN, OUTPUT);    digitalWrite(fanOutPIN, HIGH);
  pinMode(buttonPIN, INPUT_PULLUP);
  
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  dht.begin();
  
  ticker.attach(0.6, tick);

  lcd.setCursor(0,0);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print("     Scan Wifi      ");
  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(0,3);
  lcd.print("                    ");

  const char *hostname = "WifiFishSun";

  WiFiManager wifiManager;

  //reset saved settings
  //wifiManager.resetSettings();
    
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //timeout - this will quit WiFiManager if it's not configured in 3 minutes, causing a restart
  wifiManager.setConfigPortalTimeout(180);

  //custom params
  EEPROM.begin(512);
  EEPROM.get(0, settings);
  EEPROM.get(100, TempLOW);
  EEPROM.get(110, TempHIGH);
  EEPROM.get(120, HumiLOW);
  EEPROM.get(130, HumiHIGH);
  EEPROM.end();

  if (settings.salt != EEPROM_SALT) {
    Serial.println("Invalid settings in EEPROM, trying with defaults");
    WMSettings defaults;
    settings = defaults;
  }

  Serial.println(settings.blynkToken);
  Serial.println(settings.blynkServer);
  Serial.println(settings.blynkPort);

  WiFiManagerParameter custom_blynk_text("Blynk config. <br/> No token to disable.");
  wifiManager.addParameter(&custom_blynk_text);

  WiFiManagerParameter custom_blynk_token("blynk-token", "blynk token", settings.blynkToken, 33);
  wifiManager.addParameter(&custom_blynk_token);

  WiFiManagerParameter custom_blynk_server("blynk-server", "blynk server", settings.blynkServer, 33);
  wifiManager.addParameter(&custom_blynk_server);

  WiFiManagerParameter custom_blynk_port("blynk-port", "blynk port", settings.blynkPort, 6);
  wifiManager.addParameter(&custom_blynk_port);

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  if (!wifiManager.autoConnect(hostname)) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //Serial.println(custom_blynk_token.getValue());
  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("Saving config");

    strcpy(settings.blynkToken, custom_blynk_token.getValue());
    strcpy(settings.blynkServer, custom_blynk_server.getValue());
    strcpy(settings.blynkPort, custom_blynk_port.getValue());

    Serial.println(settings.blynkToken);
    Serial.println(settings.blynkServer);
    Serial.println(settings.blynkPort);

    EEPROM.begin(512);
    EEPROM.put(0, settings);
    EEPROM.end();
  }

   //config blynk
  if (strlen(settings.blynkToken) == 0) {
    BLYNK_ENABLED = false;
  }
  if (BLYNK_ENABLED) {
    Blynk.config(settings.blynkToken, settings.blynkServer, atoi(settings.blynkPort));
  }
  
  //OTA
  ArduinoOTA.onStart([]() {
    Serial.println("Start OTA");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.begin();

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(statLED, HIGH);
  reconnectBlynk();
  
  lcd.setCursor(0,0);
  lcd.print("   Connect Finish   ");
  lcd.setCursor(0,1);
  lcd.print("    !! Online !!    ");
  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(0,3);
  lcd.print("      Start..       ");
  
  timer.setInterval(1000L, blinkLedWidget);
  timer.setInterval(3000L, dhtSend);
  timer.setInterval(30000L, reconnectBlynk); 
}

void loop() 
{
  if(BLYNK_ENABLED)
  {
    if (Blynk.connected()) {
      Blynk.run();
    }
  }

  ArduinoOTA.handle(); 
  timer.run();

  startPress = millis();
  
  while(digitalRead(buttonPIN) == LOW)
  {
    unsigned long duration = millis() - startPress;
    if (duration > 4000)
    {
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("    Reset Factory   ");
      lcd.setCursor(0,2);
      lcd.print("       Reboot       ");
      delay(3000);
      resetFactory();
    }
  }
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(Humi) || isnan(Temp)) 
  {
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("   Failed to read   ");
    lcd.setCursor(0,2);
    lcd.print("  from DHT sensor!  ");
    delay(3000);
    lcd.clear();
    return;
  }

  lcd.setCursor(0,0);
  lcd.print("  TEMP: ");
  lcd.print(Temp);
  lcd.print(" C  ");
  lcd.setCursor(0,1);
  lcd.print("  HUMI: ");
  lcd.print(Humi);
  lcd.print(" %RH  ");

  switch(modeChT)
  {
    case AUTO:
    {
      if(Temp >= TempHIGH)
      {
        ledHeater.off();
        digitalWrite(HeaterPIN, HIGH);
      }
      if(Temp <= TempLOW)
      {
        ledHeater.on();
        digitalWrite(HeaterPIN, LOW);
      }

      if(Humi >= HumiHIGH)
      {
        ledFanOut.on();
        digitalWrite(fanOutPIN, LOW);
      }
      if(Humi <= HumiLOW)
      {
        ledFanOut.off();
        digitalWrite(fanOutPIN, HIGH);
      }

      lcd.setCursor(0,2);
      lcd.print("ST ");
      lcd.print(TempLOW);
      lcd.print("  <->  ");
      lcd.print(TempHIGH);
      lcd.setCursor(0,3);
      lcd.print("SH ");
      lcd.print(HumiLOW);
      lcd.print("  <->  ");
      lcd.print(HumiHIGH);
      break;
    }
    case MANUAL:
    {
      lcd.setCursor(0,2);
      lcd.print("       Manual       ");
      lcd.setCursor(0,3);
      lcd.print("                    ");
      Blynk.setProperty(V7, "color", "#000000");
      Blynk.setProperty(V8, "color", "#000000");
      break;
    }
  }
  
}
