// struct tm {
//    int tm_sec;         /* วินาที,  range 0 to 59          */
//    int tm_min;         /* นาที, range 0 to 59           */
//    int tm_hour;        /* ชั่วโมง, range 0 to 23             */
//    int tm_mday;        /* วันที่, range 1 to 31  */
//    int tm_mon;         /* เดือน, range 0 to 11             */
//    int tm_year;        /* ปีคริสศักราช ตั้งแต่ 1900   */
//    int tm_wday;        /* วัน, range 0 to 6    */
//    int tm_yday;        /* วันใน 1 ปี, range 0 to 365  */
//    int tm_isdst;       /* daylight saving time             */
// };

#include <time.h>
#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>

#define WIFI_SSID   "dlink-ABD0"
#define WIFI_PASS   "yyafr68490"

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
