#include <ESP8266WiFi.h>
#include <time.h>

const char* ssid = "Your SSID";             // SSID is set
const char* password = "Your password";     // Password is set 

char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";

int timezone = 7 * 3600;
int dst = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
  Serial.println("");

  configTime(timezone, dst, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
}
void loop() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  Serial.print(p_tm->tm_hour); Serial.print(":");
  Serial.print(p_tm->tm_min); Serial.print(":");
  Serial.println(p_tm->tm_sec);
  Serial.println(ctime(&now));  
  delay(1000);
}