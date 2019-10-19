#include <time.h>
#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>

#define WIFI_SSID "6021607"
#define WIFI_PASSWORD "17401449"
#define LINE_TOKEN "ZRnhTKzrwuvOEwh4PuXVGKLHE9mqzZaJxscbCDGxl9u"

/// time
struct tm* p_tm;
char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";
int Minutes;
String times = "";

float Humidity;
float Temperature;
float Pressure;
String roomName = "Log";
bool checkHumidity = false;
bool checkTemperature = false;
bool checkPressure = false;

void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);// connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  LINE.setToken(LINE_TOKEN);

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

  // DHT22
  Humidity = 10;
  Temperature = 10;
  Pressure = 10;

  // show string value
  Serial.println("TimeNow:" + String(Minutes));
  Serial.println("Humidity: " + String(Humidity) + "%");
  Serial.println("Temperature: " + String(Temperature) + "°C");
  Serial.println("Presure : " + String(Pressure) + "Pa");

  if (Minutes % 5 == 0) {

    Serial.println("CHECK:" + String(Minutes));
    if (Humidity >= 70 && checkHumidity == false) {
      LINE.notify(roomName + "\nความชื้น เกินกว่าเกณฑ์: " + Humidity + "%");
      checkHumidity = true;
    }
    if (Temperature >= 30 && checkTemperature == false) {
      LINE.notify(roomName + "\nอุณหภูมิ เกินกำหนดที่: " + Temperature + "°C");
      checkTemperature = true;
    }
    if (Pressure >= 300 && checkPressure == false) {
      LINE.notify(roomName + "\nความดัน เกินกำหนดที่: " + Pressure + "Pa");
      checkPressure = true;
    }

  }
  else {
    Serial.println("RESRT:" + String(Minutes));
    checkHumidity = false;
    checkTemperature = false;
    checkPressure = false;
  }
  delay(1000);
}
