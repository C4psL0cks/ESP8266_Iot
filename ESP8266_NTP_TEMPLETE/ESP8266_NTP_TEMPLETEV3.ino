#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <time.h>

int timezone = 7*3600;
int dst = 0;

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println("Starting...");

    WiFiManager wifiManager;
    wifiManager.setTimeout(180);
    if(!wifiManager.autoConnect("NodeMCU-Config")) {
      Serial.println("Failed to connect and hit timeout");
      delay(3000);
      ESP.reset();
      delay(5000);
    }
    
    configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
    Serial.println("\nWaiting for time");
    while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");

}
void loop() {
  time_t now = time(nullptr);
  struct tm* p_tm=localtime(&now);
  
  Serial.println(ctime(&now));
  Serial.print(p_tm->tm_hour);Serial.print(":");
  Serial.print(p_tm->tm_min);Serial.print(":"); 
  Serial.println(p_tm->tm_sec);
  delay(1000);
}