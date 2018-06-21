#include <ESP8266WiFi.h>
#include <time.h>

const char* WIFI_SSID = "6021607";
const char* WIFI_PASS = "17401449";

int timezone = 7;
int dst = 0;

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

  while (WiFi.status() != WL_CONNECTED) {
    // Check to see if
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  configTime(timezone * 3600, dst * 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  time_t now = time(nullptr);
  Serial.println(ctime(&now));

}

void setup() {
  Serial.begin(9600);
  connect();

}

void loop() {
  time_t now = time(nullptr);
  Serial.println(ctime(&now));
  delay(1000);

}
