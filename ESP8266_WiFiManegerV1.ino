#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <FS.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

#include <TridentTD_LineNotify.h>

WiFiManager wifiManager;

char blynk_token[35] = "";
char line_token[45] = "";
char WIFISSID[20] = "";
char WIFIPASS[20] = "";
void Line_Notify(String message) ;
int buttonStatelock, door = 1;
//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to sav config
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

  //clean FS, for testing
  SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    //Serial.println("mounted file system");
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
          strcpy(line_token, json["line_token"]);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length

  WiFiManagerParameter custom_blynk_token("BLYNK", "blynk token", blynk_token, 35);
  WiFiManagerParameter custom_line_token("LINE", "line token", line_token, 45);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  //WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //set static ip
  //wifiManager.setSTAStaticIPConfig(IPAddress(192, 168, 1, 99), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));

  //add all your parameters here

  wifiManager.addParameter(&custom_blynk_token);
  wifiManager.addParameter(&custom_line_token);

  //reset settings - for testing
  wifiManager.resetSettings();


  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  wifiManager.setMinimumSignalQuality();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("TEST", "password")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep

    ESP.reset();
    delay(5000);
  }
  wifiManager.getSSID().toCharArray(WIFISSID, 20);
  wifiManager.getPassword().toCharArray(WIFIPASS, 20);
  //  Serial.println("******************************");
  //  Serial.println(WIFISSID);
  //  Serial.println(WIFIPASS);
  //  Serial.println("******************************");

  //if you get here you have connected to the WiFi
  Serial.println("connected...sucessfull:)");


  //read updated parameters

  strcpy(blynk_token, custom_blynk_token.getValue());
  strcpy(line_token, custom_line_token.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    json["WIFISSID"] = WIFISSID;
    json["WIFIPASS"] = WIFIPASS;
    json["blynk_token"] = blynk_token;
    json["line_token"] = line_token;



    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println(blynk_token);
  Blynk.begin(blynk_token, WIFISSID, WIFIPASS, "blynk-cloud.com", 8442);

}
BLYNK_WRITE(V1) {
  buttonStatelock = param.asInt();
  if (buttonStatelock == 0) {
   LINE.notify(" 1 : OFF");
  }
  else if (buttonStatelock == 1) {
    LINE.notify(" 1 : ON");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  Blynk.virtualWrite(10, buttonStatelock);
  Blynk.virtualWrite(11, door);

}
