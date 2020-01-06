#include <time.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <TridentTD_LineNotify.h>

#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWireBus = 4;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

#define WIFI_SSID   "6021607"
#define WIFI_PASS   "17401449"
#define FIREBASE_HOST "database-ionic-b75b9.firebaseio.com"
#define FIREBASE_AUTH "UU5t80yfVMjP9AuGfVE3hCAxrvHcUBwbA0QA0wMa"
#define LINE_TOKEN "Pxyqm9ugwNVtIsWdPoHNXyib2b6crb7cSRPRKrWgiR3"

/// time
struct tm* p_tm;
char ntp_server1[20] = "ntp.ku.ac.th";
char ntp_server2[20] = "fw.eng.ku.ac.th";
char ntp_server3[20] = "time.uni.net.th";
int sec;
String times = "";

bool checkTemperature = false;

void setup() {
  Serial.begin(115200);
  // Start the DS18B20 sensor
  sensors.begin();
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
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  LINE.setToken(LINE_TOKEN);

  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  LINE.notify("รอการเชื่อมต่อสักครู่...");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
}

void loop() {

  time_t time_now = time(nullptr);   // Time
  p_tm = localtime(&time_now);
  sec = p_tm->tm_min; //tm_min
  times = ctime(&time_now);

  sensors.requestTemperatures();
  float Temperature = sensors.getTempCByIndex(0);

  // show string value
  //Serial.println("TimeNow:" + String(sec));
  Serial.println("Temperature: " + String(Temperature) + "°C");

  Firebase.setFloat("/logDS1/temperature/", Temperature);
  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /number failed:");
    Serial.println(Firebase.error());
    return;
  }

  if ((sec % 5) == 0) {
    //Serial.println("SEND:" + String(sec));
    if (Temperature >= 25 && checkTemperature == false) {
      LINE.notify("\nอุณหภูมิ เกินกำหนดที่: " + String(Temperature) + "°C");
      checkTemperature = true;
    }
  }
  else {
    //Serial.println("RESRT:" + String(sec));
    checkTemperature = false;
  }
  delay(1000);
}
