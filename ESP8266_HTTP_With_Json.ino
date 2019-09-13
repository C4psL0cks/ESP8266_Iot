#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define WIFI_SSID "--------------------"
#define WIFI_PASS "--------------------"

void connect()
{
  // Connect to Wifi.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to.. ");
  Serial.println(WIFI_SSID);
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n", SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
}

void setup()
{

  Serial.begin(115200);
  while (!Serial){}
  connect();
}

void loop()
{

  if (WiFi.status() == WL_CONNECTED)
  { //Check WiFi connection status

    StaticJsonBuffer<300> JSONbuffer; //Declaring static JSON buffer
    JsonObject &JSONencoder = JSONbuffer.createObject();

    JSONencoder["sensorType"] = "Temperature";

    JSONencoder["humidity"] = "1";
    JSONencoder["tempC"] = "2";
    JSONencoder["tempF"] = "3";
    JSONencoder["heatIndexC"] = "4";
    JSONencoder["heatIndexF"] = "5";

    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);

    HTTPClient http; //Declare object of class HTTPClient

    http.begin("http://439ad51f.ngrok.io/code/node/test.php"); //Specify request destination
    http.addHeader("Content-Type", "application/json");        //Specify content-type header

    int httpCode = http.POST(JSONmessageBuffer); //Send the request
    String payload = http.getString();           //Get the response payload

    Serial.println(httpCode); //Print HTTP return code
    Serial.println(payload);  //Print request response payload

    http.end(); //Close connection
  }
  else
  {

    Serial.println("Error in WiFi connection");
  }

  delay(10000); //Send a request every 30 seconds
}
