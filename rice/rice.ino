#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include <BlynkSimpleEsp8266.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <TridentTD_LineNotify.h>
#define BLYNK_PRINT Serial
#define WIFI_SSID  "6021607"
#define WIFI_PASSWORD "17401449"
#define BLYNKTOKEN  "ccce911de3544679b887487197721c61"
#define LINETOKEN  "iELJyY0I2bbTsMvEW2ODsht1KVsfzysNfdxrghBFvMu"
#define EEPROM_SALT 12663

HTTPClient http;
WiFiClient client;

typedef struct {
  int salt = EEPROM_SALT;
} WMSettings;
WMSettings settings;

const int pingPin = D1;
const int inPin = D2;
long duration, cm;

char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";

int rice_age_start_insert = 0;      // วันเริ่มต้น
int rice_age_start_count = 0;       // แสดงวันที่นับต่อ
int rice_age_start_count_save = 0;  // วันที่นับต่อแล้วเอาไปเซฟเวลาไฟดับ
int count_day = 0;                  // วันที่เพิ่มขึ้น
int water_level_start_insert = 0;   // ระดับน้ำที่เพิ่ม
int daynow = 0;                     // วันปัจุบัน
int Reset;                          // รีเซต
bool checkrob1 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
bool checkrob2 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
bool checkrob3 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
bool checkrob4 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
bool checkrob5 = false;                  // กำหนด ค่าสถานะการเช้ครอบ

/// ส่งค่าไปยัง google sheets
int value1 = 0;
int value2 = 0;
int value3 = 0;
int value4 = 0;
int value5 = 0;
String timesenddata = "";
String timesendatacheck = "";
bool senddata = false;

/// เวลาปกติ
String timeNow = "";
String timecheck = "";
bool go_to_set_count = false;
bool go_to_reset = false;

/// ตัวแปลการแจ้งเตือน
bool RED_FULL_LOW = false;
bool YELLOW_MINI_LOW = false;
bool GREEN_NORMAL = false;
bool YELLOW_MINI_HIGH = false;
bool RED_FULL_HIGH = false;

bool BLYNK_ENABLED = true;
bool LINE_ENABLED = true;


WidgetLED LED1_RED_FULL_LOW(V11);
WidgetLED LED2_YELLOW_MINI_LOW(V12);
WidgetLED LED3_GREEN_NORMAL(V13);
WidgetLED LED4_YELLOW_MINI_HIGH(V14);
WidgetLED LED5_RED_FULL_HIGH(V15);

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
  Blynk.syncVirtual(V6);
  Blynk.syncVirtual(V7);
  Blynk.syncVirtual(V8);
  Blynk.syncVirtual(V9);
  Blynk.syncVirtual(V10);
  Blynk.syncVirtual(V11);
  Blynk.syncVirtual(V12);
  Blynk.syncVirtual(V13);
  Blynk.syncVirtual(V14);
  Blynk.syncVirtual(V15);
  Blynk.syncAll();
}

BLYNK_WRITE(V2) {     // อายุข้าวเริ่มต้น
  Serial.println("************************************");
  rice_age_start_insert = param.asInt();
  rice_age_start_count = rice_age_start_insert; // อายุข้าวปัจุบัน  = อายุข้าวเริ่มต้น
  Serial.println("GET_APP_RICE_AGE_START_INSERT : " + String(rice_age_start_insert));
}
BLYNK_WRITE(V3) {    // อายุข้าวปัจุบัน
  rice_age_start_count = param.asInt();
  Serial.println("GET_APP_RICE_AGE :" + String(rice_age_start_count));
}
BLYNK_WRITE(V4) {    // จำนวนวันเริ่มต้นจากอายุข้าว
  count_day = param.asInt();
  Serial.println("GET_APP_COUNT_DAY :" + String(count_day));
}
BLYNK_WRITE(V5) {    // ระดับที่เพิ่ม
  water_level_start_insert = param.asInt();
  Serial.println("GET_APP_WATER_LEVEL_START_INSERT :" + String(water_level_start_insert));
}
BLYNK_WRITE(V7) {    // วันที่ปัจุบัน
  daynow = param.asInt();
  Serial.println("GET_APP_DAY_NOW :" + String(daynow));
  Serial.println("************************************");
}
BLYNK_WRITE(V9) {    // RESET
  Reset = param.asInt();
  if (Reset == 0) {
    go_to_reset = true;
  }
}
void CALL_RED_FULL_LOW() {  // ช่วง ต่ำมาก
  LED1_RED_FULL_LOW.on();
  LED2_YELLOW_MINI_LOW.off();
  LED3_GREEN_NORMAL.off();
  LED4_YELLOW_MINI_HIGH.off();
  LED5_RED_FULL_HIGH.off();
  RED_FULL_LOW = true;
  YELLOW_MINI_LOW = false;
  GREEN_NORMAL = false;
  YELLOW_MINI_HIGH = false;
  RED_FULL_HIGH = false;
}
void CALL_YELLOW_MINI_LOW() { // ช่วง ต่ำ
  LED1_RED_FULL_LOW.off();
  LED2_YELLOW_MINI_LOW.on();
  LED3_GREEN_NORMAL.off();
  LED4_YELLOW_MINI_HIGH.off();
  LED5_RED_FULL_HIGH.off();
  RED_FULL_LOW = false;
  YELLOW_MINI_LOW = true;
  GREEN_NORMAL = false;
  YELLOW_MINI_HIGH = false;
  RED_FULL_HIGH = false;
}
void CALL_GREEN_NORMAL() { // ช่วง ปกติ
  LED1_RED_FULL_LOW.off();
  LED2_YELLOW_MINI_LOW.off();
  LED3_GREEN_NORMAL.on();
  LED4_YELLOW_MINI_HIGH.off();
  LED5_RED_FULL_HIGH.off();
  RED_FULL_LOW = false;
  YELLOW_MINI_LOW = false;
  GREEN_NORMAL = true;
  YELLOW_MINI_HIGH = false;
  RED_FULL_HIGH = false;
}
void CALL_YELLOW_MINI_HIGH() { // ช่วง  สูง
  LED1_RED_FULL_LOW.off();
  LED2_YELLOW_MINI_LOW.off();
  LED3_GREEN_NORMAL.off();
  LED4_YELLOW_MINI_HIGH.on();
  LED5_RED_FULL_HIGH.off();
  RED_FULL_LOW = false;
  YELLOW_MINI_LOW = false;
  GREEN_NORMAL = false;
  YELLOW_MINI_HIGH = true;
  RED_FULL_HIGH = false;
}
void CALL_RED_FULL_HIGH() {  // ช่วง สูงมาก
  LED1_RED_FULL_LOW.off();
  LED2_YELLOW_MINI_LOW.off();
  LED3_GREEN_NORMAL.off();
  LED4_YELLOW_MINI_HIGH.off();
  LED5_RED_FULL_HIGH.on();
  RED_FULL_LOW = false;
  YELLOW_MINI_LOW = false;
  GREEN_NORMAL = false;
  YELLOW_MINI_HIGH = false;
  RED_FULL_HIGH = true;
}
long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
void resetFactory() {
  int setzero = 0;
  Blynk.virtualWrite(V2, setzero);            // ตัวเพิ่มวัน
  Blynk.virtualWrite(V3, setzero);            // ตัวแสดงและนับวัน
  Blynk.virtualWrite(V4, setzero);            // วันวันทีเพิ่มขึ้นมา
  Blynk.virtualWrite(V5, setzero);            // ตัวเพิ่มระดับน้ำ
  Blynk.virtualWrite(V6, setzero);            // ตัวแสดงระดับน้ำ
  Blynk.virtualWrite(V7, setzero);            // แสดงวันปัจุบัน
  Serial.println("ResetFactory");
  LINE.notify("รีเซ็ตค่าในอุปกรณ์เรียบร้อยแล้ว :)");
  delay(500);
  WMSettings defaults;
  settings = defaults;
  EEPROM.begin(512);
  EEPROM.put(0, settings);
  EEPROM.put(100, setzero);                   //rice_age_start_count_save
  EEPROM.end();
  delay(500);
  WiFi.disconnect();
  delay(500);
  ESP.reset();
  delay(500);
}
void report(int value1, int value2, int value3, int value4, int value5) {

  //http://api.pushingbox.com/pushingbox?devid=vB388D4B1A917188&value1=55&value2=55&value3=55&value4=55&value5=55
  String Host = "api.pushingbox.com";
  String Deviceid = "vB388D4B1A917188";
  Serial.print("connecting to " + String(Host));
  if (!client.connect(Host, 80)) {
    Serial.println("connection failed");
    return;
  }
  String url = "/pushingbox?devid=" + Deviceid + "&value1=" + value1 + "&value2=" + value2 + "&value3=" + value3 + "&value4=" + value4 + "&value5=" + value5;
  Serial.print("Requesting URL: " + url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + Host + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  while (client.available()) {
    String line = client.readStringUntil('\r');
    //Serial.print(line);
  }
}

void setup(void) {

  Serial.begin(115200);
  EEPROM.begin(512);
  EEPROM.get(0, settings);                      // ไปดึงค่าในรอมดึงค่า setttings
  EEPROM.get(100, rice_age_start_count_save);   // ไปดึงค่าในรอมวันที่นับแล้ว และเซฟหลังจากไฟดับมานับต่อ
  EEPROM.end();
  if (settings.salt != EEPROM_SALT) {
    Serial.println("Invalid settings in EEPROM, trying with defaults");
    WMSettings defaults;
    settings = defaults;
  }
  delay(200);
  Serial.println("************************************************************");
  Serial.println("GET_RICE_AGE_START_COUNT_SAVE_FORM_ROM: " + String(rice_age_start_count_save));
  Serial.println("************************************************************");

  if (strlen(BLYNKTOKEN) == 0) {
    BLYNK_ENABLED = false;
  }
  if (BLYNK_ENABLED) {
    Blynk.begin(BLYNKTOKEN, WIFI_SSID, WIFI_PASSWORD);
  }
  if (strlen(LINETOKEN) == 0) {
    LINE_ENABLED = false;
  }
  if (LINE_ENABLED) {
    LINE.setToken(LINETOKEN);
    LINE.notify("เชื่อมต่อสำเร็จ ออนไลน์แล้ว :)");
  }

  LED1_RED_FULL_LOW.on();
  LED2_YELLOW_MINI_LOW.on();
  LED3_GREEN_NORMAL.on();
  LED4_YELLOW_MINI_HIGH.on();
  LED5_RED_FULL_HIGH.on();

  rice_age_start_count = rice_age_start_count_save;   // หลังจากไฟมา count ตัวตั้งต้น ที่เท่ากับ ตัวเพิ่มจะ ถูกใส่ค่าเซฟสุดท้ายไว้เอามานับต่อแทน

  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  LINE.notify("รอการเชื่อมต่อเวลาสักครู่...สำเร็จแล้ว");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
}
void loop(void) {

  if (BLYNK_ENABLED) {
    Blynk.run();
  }
  if (go_to_reset) {                                  // เรียกฟังชั่น resetFactory();
    resetFactory();
    go_to_reset = false;
  }
  Blynk.virtualWrite(V3, rice_age_start_count);       // update หน้าแสดงค่า

  /// sensor
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  pinMode(inPin, INPUT);
  duration = pulseIn(inPin, HIGH);
  cm = microsecondsToCentimeters(duration);
  cm = (water_level_start_insert - cm);

  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  timeNow = String(p_tm->tm_mday); //min

  if (timecheck == "") {
    timecheck = timeNow;
  }
  if (timeNow == timecheck) {
    Serial.println("Day Default");
  }
  if (timeNow != timecheck) {                         // จะทำงานก็ต่อเมื่อ วันไม่เท่ากันแล้วก็คือเปลี่ยนวัน
    timecheck = timeNow;
    go_to_set_count = true;
    if (go_to_set_count) {
      Serial.println("Day Chang");
      rice_age_start_count += 1;                      // วันที่นับต่อ
      count_day += 1;                                 // และนับวันที่เพิ่มมาว่ากี่วัน
      checkrob1 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
      checkrob2 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
      checkrob3 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
      checkrob4 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
      checkrob5 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
      go_to_set_count = false;
    }
  }
  rice_age_start_count_save = rice_age_start_count;   // บันทึกค่าปัจุบันเข้าไปใน rom
  EEPROM.begin(512);
  EEPROM.put(100, rice_age_start_count_save);         // ค่าปัจุบัน เข้าไปเซฟ
  EEPROM.end();

  Blynk.virtualWrite(V0, cm);                         // ระดับน้ำจาก sensor
  Blynk.virtualWrite(V1, ctime(&now));                // วันเวลาทั้งหมด
  Blynk.virtualWrite(V3, rice_age_start_count);       // วันที่เริ่มรับต่อ
  Blynk.virtualWrite(V4, count_day);                  // จำนวนวันที่เพิ่ม
  Blynk.virtualWrite(V6, water_level_start_insert);   // ระดับน้ำ
  Blynk.virtualWrite(V7, timeNow);                    // วัน

  //Serial.println("UPDATE_SENSOR : " + String(cm));
  //Serial.println("UPDATE_RICE_AGE_START_COUNT :" + String(rice_age_start_count));
  //Serial.println("UPDATE_COUNT_DAY :" + String(count_day));
  //Serial.println("UPDATE_WATER_LEVEL_START_INSERT :" + String(water_level_start_insert));
  //Serial.println("UPDATE_DAY_NOW :" + String(timeNow));

  value1 = cm;
  value2 = rice_age_start_insert;
  value3 = rice_age_start_count;
  value4 = count_day;
  value5 = water_level_start_insert;

  timesenddata = String(p_tm->tm_min); //min //tm_hour
  if ( timesenddata  == "") {
    timesenddata = timesendatacheck;
  }
  if (timesenddata != timesendatacheck) {
    timesendatacheck = timesenddata;
    senddata = true;
    if (senddata) {
      report(value1, value2, value3, value4, value5);
      Serial.println("Senddata");
      delay(2000);
      senddata = false;
    }
  }

  //case 1-20 day no water 0 cm
  if (rice_age_start_count >= 0 && rice_age_start_count <= 20) {
    if (cm <= 0) {               // ช่วง -0 - 0 ปกติ
      CALL_GREEN_NORMAL();
    }
    if ((cm >= 3) && (cm <= 5)) { // ช่วง 0 - 5 สูง
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 5) {                 // ช่วง 5 ขึ้นไป สูงมาก
      CALL_RED_FULL_HIGH();
    }
    if ((GREEN_NORMAL == true) && (checkrob3 == false)) {
      Serial.println("GREEN_NORMAL");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!!");
      checkrob3 = true;
    }
    if ((YELLOW_MINI_HIGH == true ) && ( checkrob4 == false)) {
      Serial.println("YELLOW_MINI_HIGH");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!!");
      checkrob4 = true;
    }
    if ((RED_FULL_HIGH == true) && (checkrob5 == false)) {
      Serial.println("RED_FULL_HIGH");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!!");
      checkrob5 = true;
    }
  }

  //case 21 - 25 day use water 5 cm
  if ((rice_age_start_count >= 21 ) && (rice_age_start_count <= 25)) {
    if (cm <= 0) {               // ช่วง -0 - 0 ต่ำมาก
      CALL_RED_FULL_LOW();
    }
    if ((cm > 0) && (cm <= 2)) {  //ช่วง 0 - 3 ต่ำ
      CALL_YELLOW_MINI_LOW();
    }
    if ((cm >= 3) && (cm <= 5)) {  //ช่วง 3 - 5 ปกติ
      CALL_GREEN_NORMAL();
    }
    if ((cm >= 6) && (cm <= 10)) { //ช่วง 6 - 10 สูง
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 11) {              // ช่วง 11 ขึ้นไป สูงมาก
      CALL_RED_FULL_HIGH();
    }
    if ((RED_FULL_LOW == true) && (checkrob1 == false)) {
      Serial.println("RED_FULL_LOW");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำมาก!!");
      checkrob1 = true;
    }
    if ((YELLOW_MINI_LOW == true) && (checkrob2 == false)) {
      Serial.println("YELLOW_MINI_LOW");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำ!!");
      checkrob2 = true;
    }
    if ((GREEN_NORMAL == true) && (checkrob3 == false)) {
      Serial.println("GREEN_NORMAL");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!!");
      checkrob3 = true;
    }
    if ((YELLOW_MINI_HIGH == true) && (checkrob4 == false)) {
      Serial.println("YELLOW_MINI_HIGH");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!!");
      checkrob4 = true;
    }
    if ((RED_FULL_HIGH == true) && (checkrob5 == false)) {
      Serial.println("RED_FULL_HIGH");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!!");
      checkrob5 = true;
    }
  }
  //case 26-35 day use water 5-10 cm
  if ((rice_age_start_count >= 26) && (rice_age_start_count <= 35)) {
    if (cm <= 0) {                // ช่วง -0-0 ต่ำมาก
      CALL_RED_FULL_LOW();
    }
    if ((cm > 0) && (cm <= 5)) { // ช่วง 0 - 5 ต่ำ
      CALL_YELLOW_MINI_LOW();
    }
    if ((cm >= 6) && (cm <= 10)) { // ช่วง 6 - 10 ปกติ
      CALL_GREEN_NORMAL();
    }
    if ((cm >= 11) && (cm <= 15)) { // ช่วง 11 - 15 สูง
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 16) {                 // ช่วง 16 ขึ้นไป สูงมาก
      CALL_RED_FULL_HIGH();
    }
    if ((RED_FULL_LOW == true) && (checkrob1 == false)) {
      Serial.println("RED_FULL_LOW");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำมาก!!");
      checkrob1 = true;
    }
    if ((YELLOW_MINI_LOW == true) && (checkrob2 == false)) {
      Serial.println("YELLOW_MINI_LOW");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำ!!");
      checkrob2 = true;
    }
    if ((GREEN_NORMAL == true) && (checkrob3 == false)) {
      Serial.println("GREEN_NORMAL");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!!");
      checkrob3 = true;
    }
    if ((YELLOW_MINI_HIGH == true) && (checkrob4 == false)) {
      Serial.println("YELLOW_MINI_HIGH");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!!");
      checkrob4 = true;
    }
    if ((RED_FULL_HIGH == true) && (checkrob5 == false)) {
      Serial.println("RED_FULL_HIGH");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!!");
      checkrob5 = true;
    }
  }
  //case 36-45 day no water 0 cm
  if ((rice_age_start_count >= 36) && (rice_age_start_count <= 45)) {
    if (cm <= 0) {               // ช่วง -0 - 0 ปกติ
      CALL_GREEN_NORMAL();
    }
    if ((cm >= 3) && (cm <= 5)) { // ช่วง 0 - 5 สูง
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 5) {                 // ช่วง 5 ขึ้นไป สูงมาก
      CALL_RED_FULL_HIGH();
    }
    if ((GREEN_NORMAL == true) && (checkrob3 == false)) {
      Serial.println("GREEN_NORMAL");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!!");
      checkrob3 = true;
    }
    if ((YELLOW_MINI_HIGH == true ) && ( checkrob4 == false)) {
      Serial.println("YELLOW_MINI_HIGH");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!!");
      checkrob4 = true;
    }
    if ((RED_FULL_HIGH == true) && (checkrob5 == false)) {
      Serial.println("RED_FULL_HIGH");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!!");
      checkrob5 = true;
    }
  }
  //case 46-90 day use water 5 -10 cm
  if ((rice_age_start_count >= 46) && (rice_age_start_count <= 90)) {
    if (cm <= 0) {                // ช่วง -0-0 ต่ำมาก
      CALL_RED_FULL_LOW();
    }
    if ((cm > 0) && (cm <= 5)) { // ช่วง 0 - 5 ต่ำ
      CALL_YELLOW_MINI_LOW();
    }
    if ((cm >= 6) && (cm <= 10)) { // ช่วง 6 - 10 ปกติ
      CALL_GREEN_NORMAL();
    }
    if ((cm >= 11) && (cm <= 15)) { // ช่วง 11 - 15 สูง
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 16) {                 // ช่วง 16 ขึ้นไป สูงมาก
      CALL_RED_FULL_HIGH();
    }
    if ((RED_FULL_LOW == true) && (checkrob1 == false)) {
      Serial.println("RED_FULL_LOW");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำมาก!!");
      checkrob1 = true;
    }
    if ((YELLOW_MINI_LOW == true) && (checkrob2 == false)) {
      Serial.println("YELLOW_MINI_LOW");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำ!!");
      checkrob2 = true;
    }
    if ((GREEN_NORMAL == true) && (checkrob3 == false)) {
      Serial.println("GREEN_NORMAL");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!!");
      checkrob3 = true;
    }
    if ((YELLOW_MINI_HIGH == true) && (checkrob4 == false)) {
      Serial.println("YELLOW_MINI_HIGH");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!!");
      checkrob4 = true;
    }
    if ((RED_FULL_HIGH == true) && (checkrob5 == false)) {
      Serial.println("RED_FULL_HIGH");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!!");
      checkrob5 = true;
    }
  }
  //case 91 -120 day  no water 0 cm
  if ((rice_age_start_count >= 91) && (rice_age_start_count <= 120)) {
    if (cm <= 0) {               // ช่วง -0 - 0 ปกติ
      CALL_GREEN_NORMAL();
    }
    if ((cm >= 3) && (cm <= 5)) { // ช่วง 0 - 5 สูง
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 5) {                 // ช่วง 5 ขึ้นไป สูงมาก
      CALL_RED_FULL_HIGH();
    }
    if ((GREEN_NORMAL == true) && (checkrob3 == false)) {
      Serial.println("GREEN_NORMAL");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!!");
      checkrob3 = true;
    }
    if ((YELLOW_MINI_HIGH == true ) && ( checkrob4 == false)) {
      Serial.println("YELLOW_MINI_HIGH");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!!");
      checkrob4 = true;
    }
    if ((RED_FULL_HIGH == true) && (checkrob5 == false)) {
      Serial.println("RED_FULL_HIGH");
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!!");
      checkrob5 = true;
    }
  }
  delay(1000);
}
