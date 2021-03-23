#include <time.h>
#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TridentTD_LineNotify.h>

#define VT_PIN A0
#define FIREBASE_HOST   "db-ionics-tester.firebaseio.com"
#define FIREBASE_AUTH   "QoZE7t6nSIHXKSW7BqQQZvgLyXvhdow4oFKILM8d"
#define WIFI_SSID       "dlink-ABD0"
#define WIFI_PASSWORD   "yyafr68490"
#define LINE_TOKEN      "VOwca6uoY6T5vR6A43J7zwg2PtOWG2z4wGvgTLFsN3u"

FirebaseData fbdo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

boolean check_round = false;
struct tm* p_tm;
char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";
String times = "";
int Minutes;
int vt_temps = 0;
int vt_temp = 0;

void setup() {

  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Hello, world!");
  lcd.clear();

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  LINE.setToken(LINE_TOKEN);

  LINE.notify("เริ่มการทำงาน...");

  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
}

void loop() {

  // Time
  time_t time_now = time(nullptr);
  p_tm = localtime(&time_now);
  Minutes = p_tm->tm_min;
  times = ctime(&time_now);

  vt_temp = analogRead(VT_PIN);
  Serial.println("VALUE PH: " + String(analogRead(VT_PIN)));

  if (vt_temp > 0 && vt_temp < 640) {
    vt_temps = constrain(vt_temps, 0, 640);
    vt_temps =  map(vt_temp, 0, 640, 0, 4); // น้ำส้ม 511
    Serial.println("VALUE PH: " + String(vt_temps));
    lcd.setCursor(3, 0);
    lcd.print("SENSOR PH");
    lcd.setCursor(0, 1);
    lcd.print("VALUE PH:");
    lcd.setCursor(9, 1);
    lcd.print(vt_temps);
    lcd.print("       ");
  }
  else if (vt_temp > 650 && vt_temp < 740) {
    vt_temps = constrain(vt_temps, 650, 740);
    vt_temps =  map(vt_temp, 650, 740, 5, 7); // น้ำเปล่า 700
    Serial.println("VALUE PH: " + String(vt_temps));
    lcd.setCursor(3, 0);
    lcd.print("SENSOR PH");
    lcd.setCursor(0, 1);
    lcd.print("VALUE PH:");
    lcd.setCursor(9, 1);
    lcd.print(vt_temps);
    lcd.print("       ");
  }
  else if (vt_temp > 750 && vt_temp < 1024) {
    vt_temps = constrain(vt_temps, 750, 1024);
    vt_temps =  map(vt_temp, 750, 1024, 8, 14); // น้ำสบู่
    Serial.println("VALUE PH: " + String(vt_temps));
    lcd.setCursor(3, 0);
    lcd.print("SENSOR PH");
    lcd.setCursor(0, 1);
    lcd.print("VALUE PH:");
    lcd.setCursor(9, 1);
    lcd.print(vt_temps);
    lcd.print("       ");
  }

  if (Minutes % 5 == 0) {
    Serial.println("CHECK:" + String(Minutes));
    if (vt_temps < 5 && check_round == false) {
      LINE.notify("ค่า pH ต่ำ :" + String(vt_temps));
      check_round = true;
    }
    else if (vt_temps > 7 && check_round == false) {
      LINE.notify("ค่า pH สูง :" + String(vt_temps));
      check_round = true;
    }
  }
  else {
    Serial.println("RESRT:" + String(Minutes));
    check_round = false;
  }

  if (Firebase.setInt(fbdo, "/esp8266/value/", vt_temps)) {
    //    Serial.println("Set int data success");
  } else {
    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }
  delay(300);
}
