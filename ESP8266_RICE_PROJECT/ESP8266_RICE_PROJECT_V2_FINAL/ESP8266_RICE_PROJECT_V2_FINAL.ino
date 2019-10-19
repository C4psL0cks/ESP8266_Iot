#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266HTTPClient.h>
#include <TridentTD_LineNotify.h>
#define BLYNK_PRINT Serial
#define WIFI_SSID  "6021607"
#define WIFI_PASSWORD "17401449"
#define BLYNKTOKEN  "2542d4b02a2b4b2a9b707b9ec4ae4aaa"
#define LINETOKEN  "iELJyY0I2bbTsMvEW2ODsht1KVsfzysNfdxrghBFvMu"
#define EEPROM_SALT 12663
#define pingPin D1
#define inPin D2
#define LED  D4

HTTPClient http;
WiFiClient client;

typedef struct {
  int salt = EEPROM_SALT;
} WMSettings;
WMSettings settings;

/// Sensor
long duration, cm;

/// NTP time
char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";

/// App
int rice_age_start_insert = 0;      // วันเริ่มต้น
int rice_age_start_count = 0;       // แสดงวันที่นับต่อ
int rice_age_start_count_save = 0;  // วันที่นับต่อแล้วเอาไปเซฟเวลาไฟดับ
int count_day = 0;                  // วันที่เพิ่มขึ้น
int water_level_start_insert = 0;   // ระดับน้ำที่เพิ่ม
int daynow = 0;                     // วันปัจุบัน
int Reset;                          // รีเซต

/// Check
bool checkrob1 = false, checkrob2 = false, checkrob3 = false, checkrob4 = false, checkrob5 = false;

/// ส่งค่าไปยัง google sheets
int value1 = 0, value2 = 0, value3 = 0, value4 = 0, value5 = 0;
String timesenddata = "";
String timesendatacheck = "";
bool senddata = false;

/// Time Defult
String timeNow = "";
String timecheck = "";
bool go_to_set_count = false;
bool go_to_reset = false;

/// Notify Value
bool RED_FULL_LOW = false, YELLOW_MINI_LOW = false, GREEN_NORMAL = false, YELLOW_MINI_HIGH = false, RED_FULL_HIGH = false;

/// Line value
bool BLYNK_ENABLED = true, LINE_ENABLED = true;

/// Day Notify Value
int DAY_NOTIFY_RED_FULL_LOW = 0, DAY_NOTIFY_YELLOW_MINI_LOW = 0, DAY_NOTIFY_GREEN_NORMAL = 0, DAY_NOTIFY_YELLOW_MINI_HIGH = 0, DAY_NOTIFY_RED_FULL_HIGH = 0;

/// Check rob send
bool checkrobsend1 = false, checkrobsend2 = false, checkrobsend3 = false, checkrobsend4 = false , checkrobsend5 = false;
bool checkrobsend6 = false, checkrobsend7 = false, checkrobsend8 = false, checkrobsend9 = false, checkrobsend10 = false;
bool checkrobsend11 = false, checkrobsend12 = false, checkrobsend13 = false, checkrobsend14 = false, checkrobsend15 = false;

int year = 0;
int mon = 0;
int day = 0;
int resultDays;
bool call_stamp_day = false;
String timeshow = "";
bool go_to_set_resultDays = false;

time_t time_now_epoch; // วันที่รับมาแปลงในรูปแบบ epoch
time_t now; //   วันปัจุบันในรูปแบบ epoch
struct tm* p_tm; // ประกาศ สตักเจอร์ ของระบบวันที่
struct tm time_now_tm; // ประกาศ สตักเจอร์ ของระบบวันที่
long int start_ts;  // วันที่รับมาแปลงในรูปแบบ epoch วันเริ่มต้น
long int end_ts;  // วันที่รับมาแปลงในรูปแบบ epoch วันจบที่เป็นปัจุบัญ
bool check_around = true; // เช็ครอบ
long stamp_value; // วันที่รับมาแปลงในรูปแบบ epoch
long stamp_value_save; // วันที่รับมาแปลงในรูปแบบ epoch

WidgetLED LED1_RED_FULL_LOW(V11);
WidgetLED LED2_YELLOW_MINI_LOW(V12);
WidgetLED LED3_GREEN_NORMAL(V13);
WidgetLED LED4_YELLOW_MINI_HIGH(V14);
WidgetLED LED5_RED_FULL_HIGH(V15);

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0); Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2); Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4); Blynk.syncVirtual(V5);
  Blynk.syncVirtual(V6); Blynk.syncVirtual(V7);
  Blynk.syncVirtual(V8); Blynk.syncVirtual(V9);
  Blynk.syncVirtual(V10); Blynk.syncVirtual(V11);
  Blynk.syncVirtual(V12); Blynk.syncVirtual(V13);
  Blynk.syncVirtual(V14); Blynk.syncVirtual(V15);
  Blynk.syncAll();
}

BLYNK_WRITE(V2) {     // อายุข้าวเริ่มต้น
  Serial.println("************************************");
  rice_age_start_insert = param.asInt();
  rice_age_start_count = rice_age_start_insert; // อายุข้าวปัจุบัน  = อายุข้าวเริ่มต้น
  Serial.println("GET_APP_RICE_AGE_START_INSERT : " + String(rice_age_start_insert));
  if (rice_age_start_insert == 0) {
    call_stamp_day = false;
  }
  else {
    call_stamp_day = true;
  }
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
BLYNK_WRITE(V9) {    // RESET
  Reset = param.asInt();
  if (Reset == 0) {
    go_to_reset = true;
  }
}
void CALL_RED_FULL_LOW() {  // ช่วง ต่ำมาก
  LED1_RED_FULL_LOW.on(); LED2_YELLOW_MINI_LOW.off(); LED3_GREEN_NORMAL.off(); LED4_YELLOW_MINI_HIGH.off(); LED5_RED_FULL_HIGH.off();
  RED_FULL_LOW = true; YELLOW_MINI_LOW = false; GREEN_NORMAL = false; YELLOW_MINI_HIGH = false; RED_FULL_HIGH = false;
}
void CALL_YELLOW_MINI_LOW() { // ช่วง ต่ำ
  LED1_RED_FULL_LOW.off(); LED2_YELLOW_MINI_LOW.on(); LED3_GREEN_NORMAL.off(); LED4_YELLOW_MINI_HIGH.off(); LED5_RED_FULL_HIGH.off();
  RED_FULL_LOW = false; YELLOW_MINI_LOW = true; GREEN_NORMAL = false; YELLOW_MINI_HIGH = false; RED_FULL_HIGH = false;
}
void CALL_GREEN_NORMAL() { // ช่วง ปกติ
  LED1_RED_FULL_LOW.off(); LED2_YELLOW_MINI_LOW.off(); LED3_GREEN_NORMAL.on(); LED4_YELLOW_MINI_HIGH.off(); LED5_RED_FULL_HIGH.off();
  RED_FULL_LOW = false; YELLOW_MINI_LOW = false; GREEN_NORMAL = true; YELLOW_MINI_HIGH = false; RED_FULL_HIGH = false;
}
void CALL_YELLOW_MINI_HIGH() { // ช่วง  สูง
  LED1_RED_FULL_LOW.off(); LED2_YELLOW_MINI_LOW.off(); LED3_GREEN_NORMAL.off(); LED4_YELLOW_MINI_HIGH.on(); LED5_RED_FULL_HIGH.off();
  RED_FULL_LOW = false; YELLOW_MINI_LOW = false; GREEN_NORMAL = false; YELLOW_MINI_HIGH = true; RED_FULL_HIGH = false;
}
void CALL_RED_FULL_HIGH() {  // ช่วง สูงมาก
  LED1_RED_FULL_LOW.off(); LED2_YELLOW_MINI_LOW.off(); LED3_GREEN_NORMAL.off(); LED4_YELLOW_MINI_HIGH.off(); LED5_RED_FULL_HIGH.on();
  RED_FULL_LOW = false; YELLOW_MINI_LOW = false; GREEN_NORMAL = false; YELLOW_MINI_HIGH = false; RED_FULL_HIGH = true;
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
  Blynk.virtualWrite(V7, setzero);            // ตัวแสดงระดับน้ำ
  Blynk.virtualWrite(V8, setzero);            // ตัวแสดงระดับน้ำ
  Blynk.virtualWrite(V10, setzero);            // ตัวแสดงระดับน้ำ
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
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  EEPROM.begin(512);
  EEPROM.get(0, settings);                      // ไปดึงค่าในรอมดึงค่า setttings
  EEPROM.get(100, rice_age_start_count_save);   // ไปดึงค่าในรอมวันที่นับแล้ว และเซฟหลังจากไฟดับมานับต่อ
  EEPROM.get(150, stamp_value_save);            // ไปดึงค่าในรอมวันที่นับแล้ว และเซฟหลังจากไฟดับมานับต่อ
  EEPROM.end();


  if (settings.salt != EEPROM_SALT) {
    Serial.println("Invalid settings in EEPROM, trying with defaults");
    WMSettings defaults;
    settings = defaults;
  }
  delay(200);
  Serial.println("************************************************************");
  Serial.println("GET_RICE_AGE_START_COUNT_SAVE_FORM_ROM: " + String(rice_age_start_count_save));
  Serial.println("GET_DAY_SATMP_SAVE_FORM_ROM: " + String(stamp_value_save));
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
  digitalWrite(LED, HIGH);
  LED1_RED_FULL_LOW.on();
  LED2_YELLOW_MINI_LOW.on();
  LED3_GREEN_NORMAL.on();
  LED4_YELLOW_MINI_HIGH.on();
  LED5_RED_FULL_HIGH.on();

  rice_age_start_count = rice_age_start_count_save;   // หลังจากไฟมา count ตัวตั้งต้น ที่เท่ากับ ตัวเพิ่มจะ ถูกใส่ค่าเซฟสุดท้ายไว้เอามานับต่อแทน
  stamp_value = stamp_value_save;                     // หลังจากไฟมา stamp_value = stamp_value_save แล้วเอา stamp_value ไปคำนวน

  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  LINE.notify("รอการเชื่อมต่อเวลาสักครู่...สำเร็จแล้ว");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
}
void loop(void) {

  now = time(nullptr);
  p_tm = localtime(&now);

  if (BLYNK_ENABLED) {
    Blynk.run();
  }
  if (go_to_reset) {
    resetFactory();
    go_to_reset = false;
  }
  Blynk.virtualWrite(V3, rice_age_start_count);

  year = p_tm->tm_year + 1900; //2019
  mon = p_tm->tm_mon + 1;  //05
  day = p_tm->tm_mday;   //30

  //แปลงจากวัน 2019 5 30 -> 1559174400
  time_now_tm.tm_year = year - 1900;
  time_now_tm.tm_mon = mon - 1;
  time_now_tm.tm_mday = day;
  time_now_tm.tm_isdst = -1;
  time_now_epoch = mktime(&time_now_tm);

  Blynk.virtualWrite(V7, day);
  Blynk.virtualWrite(V8, mon);
  Blynk.virtualWrite(V10, year);

  start_ts = time_now_epoch; // 1559174400 // 2019-5-30
  end_ts = now; //1559174400 // 2019-6-2

  Serial.println("epoch_Start : " + String(start_ts));
  Serial.println("epoch_End : " + String(end_ts));


  if (call_stamp_day) {
    stamp_value = start_ts; // stamp_value = 1559174400
    Serial.println("Stamp_value : " + String(stamp_value));
    stamp_value_save = stamp_value;
    EEPROM.begin(512);
    EEPROM.put(150, stamp_value_save);         // ค่าปัจุบัน เข้าไปเซฟ
    EEPROM.end();

  }

  resultDays = (end_ts - stamp_value) / (60 * 60 * 24);
  Serial.println("Result  :" + String(resultDays)); // resultday = 2

  timeNow = String(p_tm->tm_min);
  if (timecheck == "") {
    timecheck = timeNow;
  }

  // ถ้ายังเป็นวันเดิมไม่ต้องทำอะไร และ ไฟไม่ดับ
  if (timeNow == timecheck && resultDays == 0) {
    Serial.println("*************************");
    Serial.println("Day Default");
    Serial.println("DAY-RED-LOW : " + String(DAY_NOTIFY_RED_FULL_LOW));
    Serial.println("DAY-YELLOW-LOW : " + String(DAY_NOTIFY_YELLOW_MINI_LOW));
    Serial.println("DAY-GREEN : " + String(DAY_NOTIFY_GREEN_NORMAL));
    Serial.println("DAY-YELLOW-FULL : " + String(DAY_NOTIFY_YELLOW_MINI_HIGH));
    Serial.println("DAY-RED-FULL : " + String(DAY_NOTIFY_RED_FULL_HIGH));
    Serial.println("*************************");
  }
  // ถ้าวันเปลี่ยนให้ +1
  if (timeNow != timecheck) {
    timecheck = timeNow;
    go_to_set_count = true;
    if (go_to_set_count) {
      Serial.println("Day Chang");
      rice_age_start_count += 1;
      checkrob1 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
      checkrob2 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
      checkrob3 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
      checkrob4 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
      checkrob5 = false;                  // กำหนด ค่าสถานะการเช้ครอบ
      go_to_set_count = false;
    }
  }
  Serial.println("Reice Before :" + String(rice_age_start_count));

  //ถ้า start - end ไม่เท่ากับ 0 แสดงว่า ไฟดับ
  if (resultDays != 0 && check_around == true) {
    go_to_set_resultDays = true;
    if (go_to_set_resultDays) {
      Serial.println("Power off");
      //rice_age_start_count += resultDays;
      rice_age_start_count = rice_age_start_insert + resultDays;
      go_to_set_resultDays = false;
      check_around = false;
    }
  }

  Serial.println("Reice After :" + String(rice_age_start_count));

  rice_age_start_count_save = rice_age_start_count;   // บันทึกค่าปัจุบันเข้าไปใน rom
  EEPROM.begin(512);
  EEPROM.put(100, rice_age_start_count_save);         // ค่าปัจุบัน เข้าไปเซฟ
  EEPROM.end();

  Blynk.virtualWrite(V0, cm);                         // ระดับน้ำจาก sensor
  Blynk.virtualWrite(V1, ctime(&now));                // วันเวลาทั้งหมด
  Blynk.virtualWrite(V3, rice_age_start_count);       // วันที่เริ่มรับต่อ
  Blynk.virtualWrite(V4, count_day);                  // จำนวนวันที่เพิ่ม
  Blynk.virtualWrite(V6, water_level_start_insert);   // ระดับน้ำ

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
      Serial.println("Send data to Google Sheet...");
      delay(2500);
      senddata = false;
    }
  }

  //case 1-20 day no water 0 cm 20 day
  if (rice_age_start_count >= 0 && rice_age_start_count <= 20) {
    if (cm <= 0) {
      CALL_GREEN_NORMAL();
    }
    if ((cm >= 3) && (cm <= 5)) {
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 5) {
      CALL_RED_FULL_HIGH();
    }
    if ((GREEN_NORMAL == true) && (checkrob3 == false)) {
      DAY_NOTIFY_GREEN_NORMAL += 1;
      Serial.println("GREEN_NORMAL");
      if (DAY_NOTIFY_GREEN_NORMAL == 1) {
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        checkrobsend1 = false;
        checkrobsend2 = false;
        checkrobsend3 = false;
      }
      checkrob3 = true;
    }
    if ((YELLOW_MINI_HIGH == true) && (checkrob4 == false)) {
      DAY_NOTIFY_YELLOW_MINI_HIGH += 1;
      Serial.println("YELLOW_MINI_HIGH");
      if (DAY_NOTIFY_YELLOW_MINI_HIGH == 1) {
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend4 = false;
        checkrobsend5 = false;
        checkrobsend6 = false;
      }
      checkrob4 = true;
    }
    if ((RED_FULL_HIGH == true) && (checkrob5 == false)) {
      DAY_NOTIFY_RED_FULL_HIGH += 1;
      Serial.println("RED_FULL_HIGH");
      if (DAY_NOTIFY_RED_FULL_HIGH == 1) {
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        checkrobsend7 = false;
        checkrobsend8 = false;
        checkrobsend9 = false;
      }
      checkrob5 = true;
    }

    //// DAY_NOTIFY_GREEN_NORMAL
    if ((DAY_NOTIFY_GREEN_NORMAL == 1) && (checkrobsend1 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 1 วันแล้ว");
      checkrobsend1 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 7) && (checkrobsend2 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 7 วันแล้ว");
      checkrobsend2 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 14) && (checkrobsend3 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 14 วันแล้ว");
      checkrobsend3 = true;
    }

    //// DAY_NOTIFY_YELLOW_MINI_HIGH
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 1) && (checkrobsend4 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 1 วันแล้ว");
      checkrobsend4 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 7) && (checkrobsend5 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 7 วันแล้ว");
      checkrobsend5 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 14) && (checkrobsend6 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 14 วันแล้ว");
      checkrobsend6 = true;
    }

    //// DAY_NOTIFY_RED_FULL_HIGH
    if ((DAY_NOTIFY_RED_FULL_HIGH == 1) && (checkrobsend7 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 1 วันแล้ว");
      checkrobsend7 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 7) && (checkrobsend8 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 7 วันแล้ว");
      checkrobsend8 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 14) && (checkrobsend9 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 14 วันแล้ว");
      checkrobsend9 = true;
    }

    if (rice_age_start_count == 20) {
      DAY_NOTIFY_RED_FULL_LOW = 0;
      DAY_NOTIFY_YELLOW_MINI_LOW = 0;
      DAY_NOTIFY_GREEN_NORMAL = 0;
      DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
      DAY_NOTIFY_RED_FULL_HIGH = 0;
    }
  }


  //case 21 - 25 day use water 5 cm 4 day
  if ((rice_age_start_count >= 21 ) && (rice_age_start_count <= 25)) {
    if (cm <= 0) {
      CALL_RED_FULL_LOW();
    }
    if ((cm > 0) && (cm <= 2)) {
      CALL_YELLOW_MINI_LOW();
    }
    if ((cm >= 3) && (cm <= 5)) {
      CALL_GREEN_NORMAL();
    }
    if ((cm >= 6) && (cm <= 10)) {
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 11) {
      CALL_RED_FULL_HIGH();
    }
    if ((RED_FULL_LOW == true) && (checkrob1 == false)) {
      DAY_NOTIFY_RED_FULL_LOW += 1;
      Serial.println("RED_FULL_LOW");
      if (DAY_NOTIFY_RED_FULL_LOW == 1) {
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend1 = false;
      }
      checkrob1 = true;
    }
    if ((YELLOW_MINI_LOW == true) && (checkrob2 == false)) {
      DAY_NOTIFY_YELLOW_MINI_LOW += 1;
      Serial.println("YELLOW_MINI_LOW");
      if (DAY_NOTIFY_YELLOW_MINI_LOW == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend1 = false;
      }
      checkrob2 = true;
    }
    if ((GREEN_NORMAL == true) && (checkrob3 == false)) {
      DAY_NOTIFY_GREEN_NORMAL += 1;
      Serial.println("GREEN_NORMAL");
      if (DAY_NOTIFY_GREEN_NORMAL == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend2 = false;
      }
      checkrob3 = true;
    }
    if ((YELLOW_MINI_HIGH == true) && (checkrob4 == false)) {
      DAY_NOTIFY_YELLOW_MINI_HIGH += 1;
      Serial.println("YELLOW_MINI_HIGH");
      if (DAY_NOTIFY_YELLOW_MINI_HIGH == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend3 = false;
      }
      checkrob4 = true;
    }
    if ((RED_FULL_HIGH == true) && (checkrob5 == false)) {
      Serial.println("RED_FULL_HIGH");
      if (DAY_NOTIFY_RED_FULL_HIGH == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        checkrobsend4 = false;
      }
      checkrob5 = true;
    }

    //// DAY_NOTIFY_RED_FULL_LOW
    if ((DAY_NOTIFY_RED_FULL_LOW == 1) && (checkrobsend1 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำมาก!! 1 วันแล้ว");
      checkrobsend1 = true;
    }

    //// DAY_NOTIFY_GREEN_NORMAL
    if ((DAY_NOTIFY_YELLOW_MINI_LOW == 1) && (checkrobsend2 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำ!! 1 วันแล้ว");
      checkrobsend2 = true;
    }

    //// DAY_NOTIFY_GREEN_NORMAL
    if ((DAY_NOTIFY_GREEN_NORMAL == 1) && (checkrobsend3 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 1 วันแล้ว");
      checkrobsend3 = true;
    }

    //// DAY_NOTIFY_YELLOW_MINI_HIGH
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 1) && (checkrobsend4 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 1 วันแล้ว");
      checkrobsend4 = true;
    }

    //// DAY_NOTIFY_RED_FULL_HIGH
    if ((DAY_NOTIFY_RED_FULL_HIGH == 1) && (checkrobsend5 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 1 วันแล้ว");
      checkrobsend5 = true;
    }

    if (rice_age_start_count == 25) {
      DAY_NOTIFY_RED_FULL_LOW = 0;
      DAY_NOTIFY_YELLOW_MINI_LOW = 0;
      DAY_NOTIFY_GREEN_NORMAL = 0;
      DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
      DAY_NOTIFY_RED_FULL_HIGH = 0;
    }
  }


  //case 26-35 day use water 5-10 cm 9 day
  if ((rice_age_start_count >= 26) && (rice_age_start_count <= 35)) {
    if (cm <= 0) {
      CALL_RED_FULL_LOW();
    }
    if ((cm > 0) && (cm <= 5)) {
      CALL_YELLOW_MINI_LOW();
    }
    if ((cm >= 6) && (cm <= 10)) {
      CALL_GREEN_NORMAL();
    }
    if ((cm >= 11) && (cm <= 15)) {
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 16) {
      CALL_RED_FULL_HIGH();
    }

    if ((RED_FULL_LOW == true) && (checkrob1 == false)) {
      DAY_NOTIFY_RED_FULL_LOW += 1;
      Serial.println("RED_FULL_LOW");
      if (DAY_NOTIFY_RED_FULL_LOW == 1) {
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend1 = false;
        checkrobsend2 = false;
      }
      checkrob1 = true;
    }
    if ((YELLOW_MINI_LOW == true) && (checkrob2 == false)) {
      DAY_NOTIFY_YELLOW_MINI_LOW += 1;
      Serial.println("YELLOW_MINI_LOW");
      if (DAY_NOTIFY_YELLOW_MINI_LOW == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend3 = false;
        checkrobsend4 = false;
      }
      checkrob2 = true;
    }
    if ((GREEN_NORMAL == true) && (checkrob3 == false)) {
      DAY_NOTIFY_GREEN_NORMAL += 1;
      Serial.println("GREEN_NORMAL");
      if (DAY_NOTIFY_GREEN_NORMAL == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend5 = false;
        checkrobsend6 = false;
      }
      checkrob3 = true;
    }
    if ((YELLOW_MINI_HIGH == true) && (checkrob4 == false)) {
      DAY_NOTIFY_YELLOW_MINI_HIGH += 1;
      Serial.println("YELLOW_MINI_HIGH");
      if (DAY_NOTIFY_YELLOW_MINI_HIGH == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend7 = false;
        checkrobsend8 = false;
      }
      checkrob4 = true;
    }
    if ((RED_FULL_HIGH == true) && (checkrob5 == false)) {
      Serial.println("RED_FULL_HIGH");
      if (DAY_NOTIFY_RED_FULL_HIGH == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        checkrobsend9 = false;
        checkrobsend10 = false;
      }
      checkrob5 = true;
    }

    //// DAY_NOTIFY_RED_FULL_LOW
    if ((DAY_NOTIFY_RED_FULL_LOW == 1) && (checkrobsend1 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำมาก!! 1 วันแล้ว");
      checkrobsend1 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_LOW == 7) && (checkrobsend2 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำมาก!! 7 วันแล้ว");
      checkrobsend2 = true;
    }

    //// DAY_NOTIFY_YELLOW_MINI_LOW
    if ((DAY_NOTIFY_YELLOW_MINI_LOW == 1) && (checkrobsend3 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำ!! 1 วันแล้ว");
      checkrobsend3 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_LOW == 7) && (checkrobsend4 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำ!! 7 วันแล้ว");
      checkrobsend4 = true;
    }

    //// DAY_NOTIFY_GREEN_NORMAL
    if ((DAY_NOTIFY_GREEN_NORMAL == 1) && (checkrobsend5 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 1 วันแล้ว");
      checkrobsend5 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 7) && (checkrobsend6 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 7 วันแล้ว");
      checkrobsend6 = true;
    }

    //// DAY_NOTIFY_YELLOW_MINI_HIGH
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 1) && (checkrobsend7 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 1 วันแล้ว");
      checkrobsend7 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 7) && (checkrobsend8 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 7 วันแล้ว");
      checkrobsend8 = true;
    }

    //// DAY_NOTIFY_RED_FULL_HIGH
    if ((DAY_NOTIFY_RED_FULL_HIGH == 1) && (checkrobsend9 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 1 วันแล้ว");
      checkrobsend9 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 7) && (checkrobsend10 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 7 วันแล้ว");
      checkrobsend10 = true;
    }
    if (rice_age_start_count == 35) {
      DAY_NOTIFY_RED_FULL_LOW = 0;
      DAY_NOTIFY_YELLOW_MINI_LOW = 0;
      DAY_NOTIFY_GREEN_NORMAL = 0;
      DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
      DAY_NOTIFY_RED_FULL_HIGH = 0;
    }
  }


  //case 36-45 day no water 0 cm 9 day
  if ((rice_age_start_count >= 36) && (rice_age_start_count <= 45)) {
    if (cm <= 0) {
      CALL_GREEN_NORMAL();
    }
    if ((cm >= 3) && (cm <= 5)) {
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 5) {
      CALL_RED_FULL_HIGH();
    }

    if ((GREEN_NORMAL == true) && (checkrob3 == false)) {
      DAY_NOTIFY_GREEN_NORMAL += 1;
      Serial.println("GREEN_NORMAL");
      if (DAY_NOTIFY_GREEN_NORMAL == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend5 = false;
        checkrobsend6 = false;
      }
      checkrob3 = true;
    }
    if ((YELLOW_MINI_HIGH == true) && (checkrob4 == false)) {
      DAY_NOTIFY_YELLOW_MINI_HIGH += 1;
      Serial.println("YELLOW_MINI_HIGH");
      if (DAY_NOTIFY_YELLOW_MINI_HIGH == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend7 = false;
        checkrobsend8 = false;
      }
      checkrob4 = true;
    }
    if ((RED_FULL_HIGH == true) && (checkrob5 == false)) {
      Serial.println("RED_FULL_HIGH");
      if (DAY_NOTIFY_RED_FULL_HIGH == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        checkrobsend9 = false;
        checkrobsend10 = false;
      }
      checkrob5 = true;
    }

    //// DAY_NOTIFY_GREEN_NORMAL
    if ((DAY_NOTIFY_GREEN_NORMAL == 1) && (checkrobsend5 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 1 วันแล้ว");
      checkrobsend5 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 7) && (checkrobsend6 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 7 วันแล้ว");
      checkrobsend6 = true;
    }

    //// DAY_NOTIFY_YELLOW_MINI_HIGH
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 1) && (checkrobsend7 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 1 วันแล้ว");
      checkrobsend7 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 7) && (checkrobsend8 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 7 วันแล้ว");
      checkrobsend8 = true;
    }

    //// DAY_NOTIFY_RED_FULL_HIGH
    if ((DAY_NOTIFY_RED_FULL_HIGH == 1) && (checkrobsend9 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 1 วันแล้ว");
      checkrobsend9 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 7) && (checkrobsend10 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 7 วันแล้ว");
      checkrobsend10 = true;
    }
    if (rice_age_start_count == 45) {
      DAY_NOTIFY_RED_FULL_LOW = 0;
      DAY_NOTIFY_YELLOW_MINI_LOW = 0;
      DAY_NOTIFY_GREEN_NORMAL = 0;
      DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
      DAY_NOTIFY_RED_FULL_HIGH = 0;
    }
  }

  //case 46-90 day use water 5 -10 cm 44 day
  if ((rice_age_start_count >= 46) && (rice_age_start_count <= 90)) {
    if (cm <= 0) {
      CALL_RED_FULL_LOW();
    }
    if ((cm > 0) && (cm <= 5)) {
      CALL_YELLOW_MINI_LOW();
    }
    if ((cm >= 6) && (cm <= 10)) {
      CALL_GREEN_NORMAL();
    }
    if ((cm >= 11) && (cm <= 15)) {
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 16) {
      CALL_RED_FULL_HIGH();
    }

    if ((RED_FULL_LOW == true) && (checkrob1 == false)) {
      DAY_NOTIFY_RED_FULL_LOW += 1;
      Serial.println("RED_FULL_LOW");
      if (DAY_NOTIFY_RED_FULL_LOW == 1) {
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend1 = false;
        checkrobsend2 = false;
        checkrobsend3 = false;
      }
      checkrob1 = true;
    }
    if ((YELLOW_MINI_LOW == true) && (checkrob2 == false)) {
      DAY_NOTIFY_YELLOW_MINI_LOW += 1;
      Serial.println("YELLOW_MINI_LOW");
      if (DAY_NOTIFY_YELLOW_MINI_LOW == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend4 = false;
        checkrobsend5 = false;
        checkrobsend6 = false;
      }
      checkrob2 = true;
    }
    if ((GREEN_NORMAL == true) && (checkrob3 == false)) {
      DAY_NOTIFY_GREEN_NORMAL += 1;
      Serial.println("GREEN_NORMAL");
      if (DAY_NOTIFY_GREEN_NORMAL == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend7 = false;
        checkrobsend8 = false;
        checkrobsend9 = false;
      }
      checkrob3 = true;
    }
    if ((YELLOW_MINI_HIGH == true) && (checkrob4 == false)) {
      DAY_NOTIFY_YELLOW_MINI_HIGH += 1;
      Serial.println("YELLOW_MINI_HIGH");
      if (DAY_NOTIFY_YELLOW_MINI_HIGH == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend10 = false;
        checkrobsend11 = false;
        checkrobsend12 = false;
      }
      checkrob4 = true;
    }
    if ((RED_FULL_HIGH == true) && (checkrob5 == false)) {
      Serial.println("RED_FULL_HIGH");
      if (DAY_NOTIFY_RED_FULL_HIGH == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        checkrobsend13 = false;
        checkrobsend14 = false;
        checkrobsend15 = false;
      }
      checkrob5 = true;
    }

    //// DAY_NOTIFY_RED_FULL_LOW
    if ((DAY_NOTIFY_RED_FULL_LOW == 1) && (checkrobsend1 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำมาก!! 1 วันแล้ว");
      checkrobsend1 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_LOW == 7) && (checkrobsend2 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำมาก!! 7 วันแล้ว");
      checkrobsend2 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_LOW == 14) && (checkrobsend3 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำมาก!! 14 วันแล้ว");
      checkrobsend3 = true;
    }

    //// DAY_NOTIFY_YELLOW_MINI_LOW
    if ((DAY_NOTIFY_YELLOW_MINI_LOW == 1) && (checkrobsend4 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำ!! 1 วันแล้ว");
      checkrobsend4 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_LOW == 7) && (checkrobsend5 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำ!! 7 วันแล้ว");
      checkrobsend5 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_LOW == 14) && (checkrobsend6 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำ!! 14 วันแล้ว");
      checkrobsend6 = true;
    }

    //// DAY_NOTIFY_GREEN_NORMAL
    if ((DAY_NOTIFY_GREEN_NORMAL == 1) && (checkrobsend7 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 1 วันแล้ว");
      checkrobsend7 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 7) && (checkrobsend8 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 7 วันแล้ว");
      checkrobsend8 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 14) && (checkrobsend9 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 14 วันแล้ว");
      checkrobsend9 = true;
    }

    //// DAY_NOTIFY_YELLOW_MINI_HIGH
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 1) && (checkrobsend10 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 1 วันแล้ว");
      checkrobsend10 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 7) && (checkrobsend11 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 7 วันแล้ว");
      checkrobsend11 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 14) && (checkrobsend12 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 14 วันแล้ว");
      checkrobsend12 = true;
    }

    //// DAY_NOTIFY_RED_FULL_HIGH
    if ((DAY_NOTIFY_RED_FULL_HIGH == 1) && (checkrobsend13 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 1 วันแล้ว");
      checkrobsend13 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 7) && (checkrobsend14 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 7 วันแล้ว");
      checkrobsend14 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 14) && (checkrobsend15 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 14 วันแล้ว");
      checkrobsend15 = true;
    }

    if (rice_age_start_count == 90) {
      DAY_NOTIFY_RED_FULL_LOW = 0;
      DAY_NOTIFY_YELLOW_MINI_LOW = 0;
      DAY_NOTIFY_GREEN_NORMAL = 0;
      DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
      DAY_NOTIFY_RED_FULL_HIGH = 0;
    }
  }


  //case 91 -120 day  no water 0 cm 29 day
  if ((rice_age_start_count >= 91) && (rice_age_start_count <= 120)) {
    if (cm <= 0) {
      CALL_GREEN_NORMAL();
    }
    if ((cm >= 3) && (cm <= 5)) {
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 5) {
      CALL_RED_FULL_HIGH();
    }
    if ((GREEN_NORMAL == true) && (checkrob3 == false)) {
      DAY_NOTIFY_GREEN_NORMAL += 1;
      Serial.println("GREEN_NORMAL");
      if (DAY_NOTIFY_GREEN_NORMAL == 1) {
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        checkrobsend1 = false;
        checkrobsend2 = false;
        checkrobsend3 = false;
      }
      checkrob3 = true;
    }
    if ((YELLOW_MINI_HIGH == true) && (checkrob4 == false)) {
      DAY_NOTIFY_YELLOW_MINI_HIGH += 1;
      Serial.println("YELLOW_MINI_HIGH");
      if (DAY_NOTIFY_YELLOW_MINI_HIGH == 1) {
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        checkrobsend4 = false;
        checkrobsend5 = false;
        checkrobsend6 = false;
      }
      checkrob4 = true;
    }
    if ((RED_FULL_HIGH == true) && (checkrob5 == false)) {
      DAY_NOTIFY_RED_FULL_HIGH += 1;
      Serial.println("RED_FULL_HIGH");
      if (DAY_NOTIFY_RED_FULL_HIGH == 1) {
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        checkrobsend7 = false;
        checkrobsend8 = false;
        checkrobsend9 = false;
      }
      checkrob5 = true;
    }

    //// DAY_NOTIFY_GREEN_NORMAL
    if ((DAY_NOTIFY_GREEN_NORMAL == 1) && (checkrobsend1 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 1 วันแล้ว");
      checkrobsend1 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 7) && (checkrobsend2 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 7 วันแล้ว");
      checkrobsend2 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 14) && (checkrobsend3 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 14 วันแล้ว");
      checkrobsend3 = true;
    }


    //// DAY_NOTIFY_YELLOW_MINI_HIGH
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 1) && (checkrobsend4 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 1 วันแล้ว");
      checkrobsend4 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 7) && (checkrobsend5 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 7 วันแล้ว");
      checkrobsend5 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 14) && (checkrobsend6 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! 14 วันแล้ว");
      checkrobsend6 = true;
    }

    //// DAY_NOTIFY_RED_FULL_HIGH
    if ((DAY_NOTIFY_RED_FULL_HIGH == 1) && (checkrobsend7 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 1 วันแล้ว");
      checkrobsend7 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 7) && (checkrobsend8 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 7 วันแล้ว");
      checkrobsend8 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 14) && (checkrobsend9 == false)) {
      LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูงมาก!! 14 วันแล้ว");
      checkrobsend9 = true;
    }

    if (rice_age_start_count == 120) {
      DAY_NOTIFY_RED_FULL_LOW = 0;
      DAY_NOTIFY_YELLOW_MINI_LOW = 0;
      DAY_NOTIFY_GREEN_NORMAL = 0;
      DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
      DAY_NOTIFY_RED_FULL_HIGH = 0;
    }
  }
  delay(500);
}
