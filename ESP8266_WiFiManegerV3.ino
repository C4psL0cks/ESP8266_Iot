//Blynk
#include <TridentTD_LineNotify.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#define BLYNK_MAX_SENDBYTES 256
WidgetLED led1(V0);
int buttonStatelock = 0;
static bool BLYNK_ENABLED = true;
int led = 2;
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V1);
}

#include <FS.h>
#include <ESP8266WiFi.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

//WiFiManager
WiFiManager wifiManager;

char blynk_token[35] = "";
char ssid[20] = ""; // ชื่อ
char pass[20] = ""; //

//flag for saving data
bool shouldSaveConfig = false;


//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
   Serial.println(LINE.getVersion());
  LINE.setToken(LINE_TOKEN);
  
  pinMode(led, OUTPUT);
  led1.off();

  //clean FS, for testing
  SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(blynk_token, json["blynk_token"]);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read

  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 35);

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //add all your parameters here
  wifiManager.addParameter(&custom_blynk_token);

  //reset settings - for testing
  wifiManager.resetSettings();

  //in seconds
  //wifiManager.setTimeout(120);

  //seting namessid,password
  if (!wifiManager.autoConnect("Ligh Box")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...success :)");

  //read updated parameters
  strcpy(blynk_token, custom_blynk_token.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    json["blynk_token"] = blynk_token;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }
    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  Blynk.begin(blynk_token, ssid, pass); //
  Blynk.config(blynk_token);
  if (strlen(blynk_token) == 0) {
    BLYNK_ENABLED = false;
  }
}

BLYNK_WRITE(V1) {
  buttonStatelock = param.asInt();
  if (buttonStatelock == 0) {
    ledon();
    LINE.notify(" 1 : ON");
  }
  if (buttonStatelock == 1) {
    ledoff();
    LINE.notify(" 1 : OFF");
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  if (BLYNK_ENABLED) {
    Blynk.run();
  }
}
void ledon() {
  led1.on();
  String message = "ON";
  Serial.println(message);
  Blynk.notify("LED on {DEVICE_NAME}");
  digitalWrite(led, HIGH);
}
void ledoff() {
  led1.off();
  String message = "OFF";
  Serial.println(message);
  Blynk.notify("LED off {DEVICE_NAME}");
  digitalWrite(led, LOW);
  
}

