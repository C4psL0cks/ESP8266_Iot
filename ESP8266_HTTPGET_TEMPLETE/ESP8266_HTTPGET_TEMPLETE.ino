#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define WIFI_SSID   "dlink-ABD0"
#define WIFI_PASS   "yyafr68490"


HTTPClient http;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("Running!");
  Serial.println("-------------------------------------");
  // Connect to Wifi.
  Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("WiFi Connecting to %s\n", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void loop()
{

  if (WiFi.status() == WL_CONNECTED)
  {
    http.begin("http://jsonplaceholder.typicode.com/users/1");
    int HTTP_CODE = http.GET();
    if (HTTP_CODE > 0)
    {
      String payload = http.getString();
      Serial.println(payload);
    }
    http.end();
  }
  delay(30000);
}
