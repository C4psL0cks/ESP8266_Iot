#include <time.h>
#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>

#define WIFI_SSID   "6021607"
#define WIFI_PASS   "17401449"
#define LINE_TOKEN "-----------"

/// time
struct tm* p_tm;
char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";
String times = "";
int Minutes;

float Humidity;
bool checkHumidity = false;

void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("Running!");
  Serial.println("-------------------------------------");
  // Connect to Wifi.
  Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("WiFi Connecting to %s\n", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  Serial.println();

  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  LINE.notify("รอการเชื่อมต่อเวลาสักครู่...สำเร็จแล้ว");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
}

void loop() {

  // Time
  time_t time_now = time(nullptr);
  p_tm = localtime(&time_now);
  Minutes = p_tm->tm_min;
  times = ctime(&time_now);

  Humidity = 10;
  // show string value
  Serial.println("TimeNow:" + String(Minutes));
  Serial.println("Humidity: " + String(Humidity) + "%");

  if (Minutes % 5 == 0) { // 5 is number Minutes wait send
    Serial.println("CHECK:" + String(Minutes));
    if (Humidity >= 70 && checkHumidity == false) {
      LINE.notify(roomName + "\nความชื้น เกินกว่าเกณฑ์: " + Humidity + "%");
      checkHumidity = true;
    }
  }
  else {
    Serial.println("RESRT:" + String(Minutes));
    checkHumidity = false;
  }
  delay(1000);
}
