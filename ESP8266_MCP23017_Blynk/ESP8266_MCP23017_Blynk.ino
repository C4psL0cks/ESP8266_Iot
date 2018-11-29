#define BLYNK_PRINT Serial

//#include <FS.h>
#include "DHT.h"
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include <ESP8266WiFi.h>
//#include <DNSServer.h>
//#include <ESP8266WebServer.h>
//#include <WiFiManager.h>
//#include <ArduinoJson.h>
#include <BlynkSimpleEsp8266.h>


char auth[] = "df432d0b36db472c8fbb6409d41a645c";      // Token จาก App Blynk
char ssid[] = "IoT_KMITL";                          // ชื่อ Wifi
char pass[] = "88888888";                            // รหัส Wifi
char blynkServ[] = "prakitblog.com";                   // Server Blynk


/*char mqtt_server[40];
char mqtt_port[6] = "8080";
char blynk_token[34] = "YOUR_BLYNK_TOKEN";
*/

#define DHTPIN 13 
#define DHTTYPE DHT22

WidgetLED led1(V4);
WidgetLED led2(V5);
WidgetLED led3(V6);
WidgetLED led4(V7);
WidgetLED led5(V8);
WidgetLED led6(V9);
WidgetLED led7(V10);
WidgetLED led8(V11);
WidgetLED ledLink(V30);

BlynkTimer timer;
DHT dht(DHTPIN, DHTTYPE);
Adafruit_MCP23017 mcp;

//bool shouldSaveConfig = false;

/*void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}*/

BLYNK_WRITE(V0)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(pinValue == 1)
  {
    led1.on();
    mcp.digitalWrite(8, LOW);
  }else
  {
    led1.off();
    mcp.digitalWrite(8, HIGH);
  }
}

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(pinValue == 1)
  {
    led2.on();
    mcp.digitalWrite(9, LOW);
  }else
  {
    led2.off();
    mcp.digitalWrite(9, HIGH);
  }
}

BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(pinValue == 1)
  {
    led3.on();
    mcp.digitalWrite(10, LOW);
  }else
  {
    led3.off();
    mcp.digitalWrite(10, HIGH);
  }
}

BLYNK_WRITE(V3)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(pinValue == 1)
  {
    led4.on();
    mcp.digitalWrite(11, LOW);
  }else
  {
    led4.off();
    mcp.digitalWrite(11, HIGH);
  }
}

BLYNK_WRITE(V12)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(pinValue == 1)
  {
    led5.on();
    mcp.digitalWrite(12, LOW);
  }else
  {
    led5.off();
    mcp.digitalWrite(12, HIGH);
  }
}

BLYNK_WRITE(V13)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(pinValue == 1)
  {
    led6.on();
    mcp.digitalWrite(13, LOW);
  }else
  {
    led6.off();
    mcp.digitalWrite(13, HIGH);
  }
}

BLYNK_WRITE(V14)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(pinValue == 1)
  {
    led7.on();
    mcp.digitalWrite(14, LOW);
  }else
  {
    led7.off();
    mcp.digitalWrite(14, HIGH);
  }
}

BLYNK_WRITE(V15)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(pinValue == 1)
  {
    led8.on();
    mcp.digitalWrite(15, LOW);
  }else
  {
    led8.off();
    mcp.digitalWrite(15, HIGH);
  }
}

// V1 LED Widget is blinking
void blinkLedWidget()
{
  if (ledLink.getValue()) {
    ledLink.off();
  } else {
    ledLink.on();
  }

  Blynk.virtualWrite(V22, WiFi.RSSI());
}

void dhtSend()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Blynk.virtualWrite(V20, t);
  Blynk.virtualWrite(V21, h);
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

boolean stateSW1 = 0;
boolean stateSW2 = 0;
boolean stateSW3 = 0;
boolean stateSW4 = 0;
boolean stateSW5 = 0;
boolean stateSW6 = 0;
boolean stateSW7 = 0;
boolean stateSW8 = 0;

void setup()
{
  // Debug console
  Serial.begin(9600);
  mcp.begin();
  mcp.pinMode(0, INPUT);
  mcp.pinMode(1, INPUT);
  mcp.pinMode(2, INPUT);
  mcp.pinMode(3, INPUT);
  mcp.pinMode(4, INPUT);
  mcp.pinMode(5, INPUT);
  mcp.pinMode(6, INPUT);
  mcp.pinMode(7, INPUT);
  mcp.pinMode(8, OUTPUT);
  mcp.pinMode(9, OUTPUT);
  mcp.pinMode(10, OUTPUT);
  mcp.pinMode(11, OUTPUT);
  mcp.pinMode(12, OUTPUT);
  mcp.pinMode(13, OUTPUT);
  mcp.pinMode(14, OUTPUT);
  mcp.pinMode(15, OUTPUT);
  mcp.digitalWrite(8, HIGH);
  mcp.digitalWrite(9, HIGH);
  mcp.digitalWrite(10, HIGH);
  mcp.digitalWrite(11, HIGH);
  mcp.digitalWrite(12, HIGH);
  mcp.digitalWrite(13, HIGH);
  mcp.digitalWrite(14, HIGH);
  mcp.digitalWrite(15, HIGH);
  dht.begin();

  //clean FS, for testing
  /*SPIFFS.format();

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

          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
          strcpy(blynk_token, json["blynk_token"]);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }

  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 32);

  WiFiManager wifiManager;

  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //set static ip
  wifiManager.setSTAStaticIPConfig(IPAddress(192,168,1,99), IPAddress(192,168,1,1), IPAddress(255,255,255,0));

  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_blynk_token);

  //reset settings - for testing
  wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();
  
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  if (!wifiManager.autoConnect("BlynkIOTBoard", "123456789")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(blynk_token, custom_blynk_token.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
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

  Blynk.config(custom_blynk_token.getValue(), custom_mqtt_server.getValue(), atoi(custom_mqtt_port.getValue()));
  */
  Blynk.begin(auth, ssid, pass, blynkServ, 8181);

  timer.setInterval(1000L, blinkLedWidget);
  timer.setInterval(2000L, dhtSend);
  timer.setInterval(30000L, reconnectBlynk); 
}

void loop()
{

  if (Blynk.connected()) {
    Blynk.run();
  }
  timer.run();

  
  if(mcp.digitalRead(0) == LOW)
  {
    delay(200);
    if(stateSW1 == 0)
    {
      stateSW1 = 1;
      led1.on();
      Blynk.virtualWrite(V0, HIGH);
      mcp.digitalWrite(8, LOW);
    }else
    {
      stateSW1 = 0;
      led1.off();
      Blynk.virtualWrite(V0, LOW);
      mcp.digitalWrite(8, HIGH);
    }
  }

  if(mcp.digitalRead(1) == LOW)
  {
    delay(200);
    if(stateSW2 == 0)
    {
      stateSW2 = 1;
      led2.on();
      Blynk.virtualWrite(V1, HIGH);
      mcp.digitalWrite(9, LOW);
    }else
    {
      stateSW2 = 0;
      led2.off();
      Blynk.virtualWrite(V1, LOW);
      mcp.digitalWrite(9, HIGH);
    }
  }

  if(mcp.digitalRead(2) == LOW)
  {
    delay(200);
    if(stateSW3 == 0)
    {
      stateSW3 = 1;
      led3.on();
      Blynk.virtualWrite(V2, HIGH);
      mcp.digitalWrite(10, LOW);
    }else
    {
      stateSW3 = 0;
      led3.off();
      Blynk.virtualWrite(V2, LOW);
      mcp.digitalWrite(10, HIGH);
    }
  }

  if(mcp.digitalRead(3) == LOW)
  {
    delay(200);
    if(stateSW4 == 0)
    {
      stateSW4 = 1;
      led4.on();
      Blynk.virtualWrite(V3, HIGH);
      mcp.digitalWrite(11, LOW);
    }else
    {
      stateSW4 = 0;
      led4.off();
      Blynk.virtualWrite(V3, LOW);
      mcp.digitalWrite(11, HIGH);
    }
  }

  if(mcp.digitalRead(4) == LOW)
  {
    delay(200);
    if(stateSW5 == 0)
    {
      stateSW5 = 1;
      led5.on();
      Blynk.virtualWrite(V12, HIGH);
      mcp.digitalWrite(12, LOW);
    }else
    {
      stateSW5 = 0;
      led5.off();
      Blynk.virtualWrite(V12, LOW);
      mcp.digitalWrite(12, HIGH);
    }
  }

  if(mcp.digitalRead(5) == LOW)
  {
    delay(200);
    if(stateSW6 == 0)
    {
      stateSW6 = 1;
      led6.on();
      Blynk.virtualWrite(V13, HIGH);
      mcp.digitalWrite(13, LOW);
    }else
    {
      stateSW6 = 0;
      led6.off();
      Blynk.virtualWrite(V13, LOW);
      mcp.digitalWrite(13, HIGH);
    }
  }

  if(mcp.digitalRead(6) == LOW)
  {
    delay(200);
    if(stateSW7 == 0)
    {
      stateSW7 = 1;
      led7.on();
      Blynk.virtualWrite(V14, HIGH);
      mcp.digitalWrite(14, LOW);
    }else
    {
      stateSW7 = 0;
      led7.off();
      Blynk.virtualWrite(V14, LOW);
      mcp.digitalWrite(14, HIGH);
    }
  }

  if(mcp.digitalRead(7) == LOW)
  {
    delay(200);
    if(stateSW8 == 0)
    {
      stateSW8 = 1;
      led8.on();
      Blynk.virtualWrite(V15, HIGH);
      mcp.digitalWrite(15, LOW);
    }else
    {
      stateSW8 = 0;
      led8.off();
      Blynk.virtualWrite(V15, LOW);
      mcp.digitalWrite(15, HIGH);
    }
  }
}
