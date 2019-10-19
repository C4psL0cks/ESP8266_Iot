#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <TridentTD_LineNotify.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

#define SSID        "--------------------"
#define PASSWORD    "--------------------"

int timeSinceLastRead = 0;
WiFiClientSecure wifiClient;

void setup()
{
  Serial.begin(115200);
  // Connect to Wifi.
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n", SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());

  LINE.setToken(LINE_TOKEN);
  Serial.println(LINE.getVersion());

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");
}
void loop()
{

  if (timeSinceLastRead > 2000)
  {

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);

    if (isnan(h) || isnan(t) || isnan(f))
    {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }
    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");
    report(h, t, f, hic, hif); //send to function
    timeSinceLastRead = 0;
  }
  delay(300);
  timeSinceLastRead += 100;
}

void report(double humidity, double tempC, double tempF, double heatIndexC, double heatIndexF)
{

  StaticJsonBuffer<300> JSONbuffer; //Declaring static JSON buffer
  JsonObject &JSONencoder = JSONbuffer.createObject();

  JSONencoder["humidity"] = humidity;
  JSONencoder["tempC"] = tempC;
  JSONencoder["tempF"] = tempF;
  JSONencoder["heatIndexC"] = heatIndexC;
  JSONencoder["heatIndexF"] = heatIndexF;

  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);
  String message = JSONmessageBuffer;

  if (message != NULL)
  {
    LINE.notify("DHT22");
    LINE.notify(message);
    LINE.notifySticker(3, 240);
    LINE.notifySticker("Hello", 1, 2);
    LINE.notifyPicture("https://xxx.jpg");
    LINE.notifyPicture("xxxx", "https://xxx.jpg");
  }
  delay(3000);
}
