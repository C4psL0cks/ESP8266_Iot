#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* WIFI_SSID = "6021607";
const char* WIFI_PASS = "17401449";

void connect() {
  // Connect to Wifi.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long wifiConnectStart = millis();

  while (WiFi.status() != WL_CONNECTED) {
    // Check to see if
    if (WiFi.status() == WL_CONNECT_FAILED) {
      Serial.println("Failed to connect to WIFI. Please verify credentials: ");
      Serial.println();
      Serial.print("SSID: ");
      Serial.println(WIFI_SSID);
      Serial.print("Password: ");
      Serial.println(WIFI_PASS);
      Serial.println();
    }

    delay(500);
    Serial.print(".");
    // Only try for 5 seconds.
    if (millis() - wifiConnectStart > 10000) {
      Serial.println("Failed to connect to WiFi");
      Serial.println("Please attempt to send updated configuration parameters.");
      return;
    }
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

}
void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(2000);
  while (!Serial) { }

  connect();
}
void loop()
{
  if(WiFi.status() != WL_CONNECTED)
  {
    connect();
  }
}
