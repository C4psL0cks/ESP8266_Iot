#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "ThingSpeak.h"
#include <TridentTD_LineNotify.h>

#define DHTPIN D4
#define DHTTYPE DHT22
#define SSID "--------------------"
#define PASSWORD "--------------------"
#define LINE_TOKEN "--------------------"

DHT dht(DHTPIN, DHTTYPE);
int timeSinceLastRead = 0;
unsigned long myChannelNumber = 495104;
const char *myWriteAPIKey = "13GJ1GJ1VIX8KXQQ";

WiFiClient client;

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
  ThingSpeak.begin(client);
  dht.begin();

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");
}
void loop()
{

  if (timeSinceLastRead > 2000)
  {
    float Humidity = dht.readHumidity();
    float Temperature = dht.readTemperature();
    float Fahrenheit = dht.readTemperature(true);

    if (isnan(Humidity) || isnan(Temperature) || isnan(Fahrenheit))
    {
      timeSinceLastRead = 0;
      return;
    }
    float hif = dht.computeHeatIndex(Fahrenheit, Humidity);
    float hic = dht.computeHeatIndex(Temperature, Humidity, false);
    Serial.print("Humidity: ");
    Serial.print(Humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(Temperature);
    Serial.print(" *C ");
    Serial.print(Fahrenheit);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");

    ThingSpeak.writeField(myChannelNumber, 1, Temperature, myWriteAPIKey);
    ThingSpeak.writeField(myChannelNumber, 2, Humidity, myWriteAPIKey);

    if (Temperature > 35)
    {
      LINE.notify("Alert now temp so hot is" + String(Temperature));
    }
    report(Humidity, Temperature, Fahrenheit, hic, hif);
    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100;
}

void report(double humidity, double tempC, double tempF, double heatIndexC, double heatIndexF)
{

  StaticJsonBuffer<300> JSONbuffer;
  JsonObject &JSONencoder = JSONbuffer.createObject();

  JSONencoder["humidity"] = humidity;
  JSONencoder["tempC"] = tempC;
  JSONencoder["tempF"] = tempF;
  JSONencoder["heatIndexC"] = heatIndexC;
  JSONencoder["heatIndexF"] = heatIndexF;

  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  //Serial.println(JSONmessageBuffer);

  Serial.println("Authenticating Device...");
  HTTPClient http;                                    //Declare object of class HTTPClient
  http.begin("http://35.198.193.206/inputjson.php");  //Specify request destination
  http.addHeader("Content-Type", "application/json"); //Specify content-type header
  http.addHeader("Accept", "application/json");

  int httpCode = http.POST(JSONmessageBuffer); //Send the request
  String payload = http.getString();           //Get the response payload

  //Serial.print("HTTPS CODE :");
  //Serial.println(httpCode);                                                              //Print HTTP return code
  Serial.print("Payload :");
  Serial.println(payload); //Print request response payload
  http.end();              //Send a request every 30 seconds
  Serial.println("Reported!");
  delay(5000);
}
