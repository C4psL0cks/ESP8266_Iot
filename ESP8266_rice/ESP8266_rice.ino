#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include <BlynkSimpleEsp8266.h>
#include <TridentTD_LineNotify.h>
#define BLYNK_PRINT Serial
#define WIFI_SSID  "6021607"
#define WIFI_PASSWORD "17401449"
#define BLYNKTOKEN  "2542d4b02a2b4b2a9b707b9ec4ae4aaa"
#define LINETOKEN  "UyNN7O9AMK6qmW8kjb7K0bK5WRVF7tE52jw1aJ77ysy"
#define EEPROM_SALT 12663
#define pingPin D1
#define inPin D2
#define LED  D4

WiFiClient client;


typedef struct {
  int salt = EEPROM_SALT;
} WMSettings;
WMSettings settings;

/// Sensor
long Distance, duration, cm;

/// NTP time
char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";

/// App
int Rice_age_start_insert = 0, Rice_age_start_count = 0, Rice_age_start_count_save = 0, Water_level_start_insert = 0, Reset, Stamp_date;
bool go_to_stamp_date = false;
bool check_around1 = false, check_around2 = false, check_around3 = false, check_around4 = false, check_around5 = false;

/// Send data to google sheets
int value1 = 0, value2 = 0, value3 = 0, value4 = 0, value5 = 0;
bool send_data = false;
String time_send_data;
String time_send_data_check;

/// Time Defult
String timeNow = "", timecheck = "";
bool go_to_set_count = false, go_to_reset = false;

/// Notify Value
bool RED_FULL_LOW = false, YELLOW_MINI_LOW = false, GREEN_NORMAL = false, YELLOW_MINI_HIGH = false, RED_FULL_HIGH = false;

/// Line value
bool BLYNK_ENABLED = true, LINE_ENABLED = true;

/// Day Notify Value
int DAY_NOTIFY_RED_FULL_LOW = 0, DAY_NOTIFY_YELLOW_MINI_LOW = 0, DAY_NOTIFY_GREEN_NORMAL = 0, DAY_NOTIFY_YELLOW_MINI_HIGH = 0, DAY_NOTIFY_RED_FULL_HIGH = 0;

/// Check rob send
bool check_around_send1 = false, check_around_send2 = false, check_around_send3 = false, check_around_send4 = false , check_around_send5 = false;
bool check_around_send6 = false, check_around_send7 = false, check_around_send8 = false, check_around_send9 = false, check_around_send10 = false;
bool check_around_send11 = false, check_around_send12 = false, check_around_send13 = false, check_around_send14 = false, check_around_send15 = false;

/// Epoch Converter day
time_t time_now_epoch, time_now;
int year = 0, mon = 0, day = 0, resultDays;
struct tm* p_tm;
struct tm time_now_tm;
bool go_to_set_resultDays = false;
bool check_around = false;
long int day_epoch_start, day_epoch_end;
long int stamp_value_epoch, stamp_value_epoch_save;

WidgetLED LED1_RED_FULL_LOW(V11);
WidgetLED LED2_YELLOW_MINI_LOW(V12);
WidgetLED LED3_GREEN_NORMAL(V13);
WidgetLED LED4_YELLOW_MINI_HIGH(V14);
WidgetLED LED5_RED_FULL_HIGH(V15);

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BLYNK_WRITE(V1) {
  Rice_age_start_insert = param.asInt();
  Rice_age_start_count = Rice_age_start_insert;
  Serial.println("GET_APP_Rice_age_start_insert : " + String(Rice_age_start_insert));
}
BLYNK_WRITE(V2) {
  Water_level_start_insert = param.asInt();
  Serial.println("GET_APP_WATER_LEVEL_START_INSERT :" + String(Water_level_start_insert));
}
BLYNK_WRITE(V3) {
  Rice_age_start_count = param.asInt();
  Serial.println("GET_APP_RICE_AGE :" + String(Rice_age_start_count));
}
BLYNK_WRITE(V7) {
  Stamp_date = param.asInt();
  if (Stamp_date == 0) {
    go_to_stamp_date = true;
  }
}
BLYNK_WRITE(V8) {
  Reset = param.asInt();
  if (Reset == 0) {
    go_to_reset = true;
  }
}

void CALL_RED_FULL_LOW() {
  LED1_RED_FULL_LOW.on(); LED2_YELLOW_MINI_LOW.off(); LED3_GREEN_NORMAL.off(); LED4_YELLOW_MINI_HIGH.off(); LED5_RED_FULL_HIGH.off();
  RED_FULL_LOW = true; YELLOW_MINI_LOW = false; GREEN_NORMAL = false; YELLOW_MINI_HIGH = false; RED_FULL_HIGH = false;
}
void CALL_YELLOW_MINI_LOW() {
  LED1_RED_FULL_LOW.off(); LED2_YELLOW_MINI_LOW.on(); LED3_GREEN_NORMAL.off(); LED4_YELLOW_MINI_HIGH.off(); LED5_RED_FULL_HIGH.off();
  RED_FULL_LOW = false; YELLOW_MINI_LOW = true; GREEN_NORMAL = false; YELLOW_MINI_HIGH = false; RED_FULL_HIGH = false;
}
void CALL_GREEN_NORMAL() {
  LED1_RED_FULL_LOW.off(); LED2_YELLOW_MINI_LOW.off(); LED3_GREEN_NORMAL.on(); LED4_YELLOW_MINI_HIGH.off(); LED5_RED_FULL_HIGH.off();
  RED_FULL_LOW = false; YELLOW_MINI_LOW = false; GREEN_NORMAL = true; YELLOW_MINI_HIGH = false; RED_FULL_HIGH = false;
}
void CALL_YELLOW_MINI_HIGH() {
  LED1_RED_FULL_LOW.off(); LED2_YELLOW_MINI_LOW.off(); LED3_GREEN_NORMAL.off(); LED4_YELLOW_MINI_HIGH.on(); LED5_RED_FULL_HIGH.off();
  RED_FULL_LOW = false; YELLOW_MINI_LOW = false; GREEN_NORMAL = false; YELLOW_MINI_HIGH = true; RED_FULL_HIGH = false;
}
void CALL_RED_FULL_HIGH() {
  LED1_RED_FULL_LOW.off(); LED2_YELLOW_MINI_LOW.off(); LED3_GREEN_NORMAL.off(); LED4_YELLOW_MINI_HIGH.off(); LED5_RED_FULL_HIGH.on();
  RED_FULL_LOW = false; YELLOW_MINI_LOW = false; GREEN_NORMAL = false; YELLOW_MINI_HIGH = false; RED_FULL_HIGH = true;
}

void ResetFactory() {
  int setzero = 0;
  Blynk.virtualWrite(V1, setzero);
  Blynk.virtualWrite(V2, setzero);
  Blynk.virtualWrite(V3, setzero);
  Blynk.virtualWrite(V4, setzero);
  Blynk.virtualWrite(V5, setzero);
  Blynk.virtualWrite(V6, setzero);
  Blynk.virtualWrite(V10, setzero);
  Serial.println("xxxxxxxxxxxxxxxxxxxxxxxxxxx");
  Serial.println("ResetFactory");
  Serial.println("xxxxxxxxxxxxxxxxxxxxxxxxxxx");
  LINE.notify("รีเซ็ตค่าในอุปกรณ์เรียบร้อยแล้ว :)");
  delay(500);
  WMSettings defaults;
  settings = defaults;
  EEPROM.begin(512);
  EEPROM.put(0, settings);
  EEPROM.put(100, setzero);                   //Rice_age_start_count_save
  EEPROM.end();
  delay(500);
  WiFi.disconnect();
  delay(500);
  ESP.reset();
  delay(500);
}

void report(int value1, int value2, int value3, int value4, int value5) {

  char serverName[] = "api.pushingbox.com";
  String Deviceid = "vB388D4B1A917188";
  String url = "/pushingbox?devid=" + Deviceid + "&value1=" + value1 + "&value2=" + value2 + "&value3=" + value3 + "&value4=" + value4 + "&value5=" + value5;
  Serial.print("Requesting URL: " + url);
  if (client.connect(serverName, 80)) {
    client.print("GET ");
    client.print(url);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(serverName);
    client.println("User-Agent: ESP8266");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("connection failed");
  }
}
void setup(void) {

  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  EEPROM.begin(512);
  EEPROM.get(0, settings);                            // ไปดึงค่าในรอมดึงค่า setttings
  EEPROM.get(100, Rice_age_start_count_save);         // ไปดึงค่าในรอมวันที่นับแล้ว และเซฟหลังจากไฟดับมานับต่อ
  EEPROM.get(150, stamp_value_epoch_save);            // ไปดึงค่าในรอมวันที่นับแล้ว และเซฟหลังจากไฟดับมานับต่อ
  EEPROM.end();

  if (settings.salt != EEPROM_SALT) {
    Serial.println("Invalid settings in EEPROM, trying with defaults");
    WMSettings defaults;
    settings = defaults;
  }
  delay(200);

  Serial.println("************************************************************");
  Serial.println("GET_Rice_age_start_count_SAVE_FORM_ROM: " + String(Rice_age_start_count_save));
  Serial.println("GET_DAY_SATMP_SAVE_FORM_ROM: " + String(stamp_value_epoch_save));
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

  Rice_age_start_count = Rice_age_start_count_save;   // หลังจากไฟมา count ตัวตั้งต้น ที่เท่ากับ ตัวเพิ่มจะ ถูกใส่ค่าเซฟสุดท้ายไว้เอามานับต่อแทน
  stamp_value_epoch = stamp_value_epoch_save;         // หลังจากไฟมา stamp_value = stamp_value_save แล้วเอา stamp_value ไปคำนวน

  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  LINE.notify("รอการเชื่อมต่อเวลาสักครู่...สำเร็จแล้ว");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
}
void loop(void) {

  time_now = time(nullptr);
  p_tm = localtime(&time_now);

  if (BLYNK_ENABLED) {
    Blynk.run();
  }
  if (go_to_reset) {
    ResetFactory();
    go_to_reset = false;
  }

  Blynk.virtualWrite(V3, Rice_age_start_count);

  pinMode(pingPin, OUTPUT); digitalWrite(pingPin, LOW);
  delayMicroseconds(2); digitalWrite(pingPin, HIGH);
  delayMicroseconds(5); digitalWrite(pingPin, LOW);
  pinMode(inPin, INPUT);
  duration = pulseIn(inPin, HIGH);
  Distance = duration * 0.174;
  cm = (Water_level_start_insert - Distance);

  if (go_to_stamp_date) {

    year = p_tm->tm_year + 1900;
    mon = p_tm->tm_mon + 1;
    day = p_tm->tm_mday;

    time_now_tm.tm_year = year - 1900;
    time_now_tm.tm_mon = mon - 1;
    time_now_tm.tm_mday = day;
    time_now_tm.tm_isdst = -1;
    time_now_epoch = mktime(&time_now_tm);

    Blynk.virtualWrite(V4, day);
    Blynk.virtualWrite(V5, mon);
    Blynk.virtualWrite(V6, year);

    day_epoch_start = time_now_epoch; //1559433600

    stamp_value_epoch = day_epoch_start;
    Serial.println("$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    Serial.println("Stamp_value_epoch : " + String(stamp_value_epoch));
    Serial.println("$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    stamp_value_epoch_save = stamp_value_epoch;
    EEPROM.begin(512);
    EEPROM.put(150, stamp_value_epoch_save);         // ค่าปัจุบัน เข้าไปเซฟ
    EEPROM.end();
    go_to_stamp_date = false;
  }

  day_epoch_end = time_now; //1559692800; //time_now;
  Serial.println("epoch_Start : " + String(day_epoch_start));
  Serial.println("epoch_End : " + String(day_epoch_end));

  resultDays = (day_epoch_end - stamp_value_epoch) / (60 * 60 * 24); // แปลงเป็นวัน
  Serial.println("Result  :" + String(resultDays));

  timeNow = String(p_tm->tm_min);
  if (timecheck == "") {
    timecheck = timeNow;
  }
  if (timeNow == timecheck) {
    Serial.println("*************************");
    Serial.println("Normal day");
    Serial.println("*************************");
  }
  if (timeNow != timecheck) {
    timecheck = timeNow;
    go_to_set_count = true;
    if (go_to_set_count) {
      Serial.println("##########################");
      Serial.println("Change day");
      Serial.println("##########################");
      check_around = false;
      check_around1 = false;
      check_around2 = false;
      check_around3 = false;
      check_around4 = false;
      check_around5 = false;
      go_to_set_count = false;
    }
  }

  if (resultDays != 0 && check_around == false) {
    go_to_set_resultDays = true;
    if (go_to_set_resultDays) {
      Serial.println("%%%%%%%%%%%%%%%%%%%%%%%%%");
      Serial.println("Day count and Power off");
      Serial.println("%%%%%%%%%%%%%%%%%%%%%%%%%");
      //อายุข้าวที่นับต่อ = วันเริ่มต้นอายุข้าว + ระยะห่างของช่วงวันที่
      Rice_age_start_count = Rice_age_start_insert + resultDays;                            
      go_to_set_resultDays = false;
      check_around = true;
    }
  }

  Serial.println("Rice_age_start_count:" + String(Rice_age_start_count));
  Rice_age_start_count_save = Rice_age_start_count;   // บันทึกค่าปัจุบันเข้าไปใน rom
  EEPROM.begin(512);
  EEPROM.put(100, Rice_age_start_count_save);         // ค่าปัจุบัน เข้าไปเซฟ
  EEPROM.end();

  Blynk.virtualWrite(V0, ctime(&time_now));
  Blynk.virtualWrite(V2, Water_level_start_insert);
  Blynk.virtualWrite(V3, Rice_age_start_count);
  Blynk.virtualWrite(V10, cm);

  value1 = cm;
  value2 = Rice_age_start_insert;
  value3 = Rice_age_start_count;
  value4 = resultDays;
  value5 = Water_level_start_insert;

  time_send_data = String(p_tm->tm_min);
  if (time_send_data  == "") {
    time_send_data = time_send_data_check;
  }
  if (time_send_data != time_send_data_check) {
    time_send_data_check = time_send_data;
    send_data = true;
    if (send_data) {
      report(value1, value2, value3, value4, value5);
      Serial.println("Send data to google sheets...");
      send_data = false;
    }
  }


  //case 0-20 day no water 0 cm 20 day
  if (Rice_age_start_count >= 0 && Rice_age_start_count <= 20) {
    if ((cm >= 0) && (cm <= 20)) {
      CALL_GREEN_NORMAL();
    }
    if ((cm > 20) && (cm <= 50)) {
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 50) {
      CALL_RED_FULL_HIGH();
    }
    if ((GREEN_NORMAL == true) && (check_around3 == false)) {
      DAY_NOTIFY_GREEN_NORMAL += 1;
      Serial.println("GREEN_NORMAL");
      if (DAY_NOTIFY_GREEN_NORMAL == 1) {
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        check_around_send1 = false;
        check_around_send2 = false;
        check_around_send3 = false;
      }
      check_around3 = true;
    }
    if ((YELLOW_MINI_HIGH == true) && (check_around4 == false)) {
      DAY_NOTIFY_YELLOW_MINI_HIGH += 1;
      Serial.println("YELLOW_MINI_HIGH");
      if (DAY_NOTIFY_YELLOW_MINI_HIGH == 1) {
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        check_around_send4 = false;
        check_around_send5 = false;
        check_around_send6 = false;
      }
      check_around4 = true;
    }
    if ((RED_FULL_HIGH == true) && (check_around5 == false)) {
      DAY_NOTIFY_RED_FULL_HIGH += 1;
      Serial.println("RED_FULL_HIGH");
      if (DAY_NOTIFY_RED_FULL_HIGH == 1) {
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        check_around_send7 = false;
        check_around_send8 = false;
        check_around_send9 = false;
      }
      check_around5 = true;
    }

    //// DAY_NOTIFY_GREEN_NORMAL
    if ((DAY_NOTIFY_GREEN_NORMAL == 1) && (check_around_send1 == false)) {
      //LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! วันแรก อายุข้าว "+String(Rice_age_start_count)+" วัน ความสูงของน้ำ "+String(cm)+" มิลลิเมตร");
      check_around_send1 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 3) && (check_around_send2 == false)) {
      //LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 3 วันแล้ว");
      check_around_send2 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 7) && (check_around_send3 == false)) {
      //LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 14 วันแล้ว");
      check_around_send3 = true;
    }

    //// DAY_NOTIFY_YELLOW_MINI_HIGH
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 1) && (check_around_send4 == false)) {
      //LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! วันที่ 1");
      check_around_send4 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 3) && (check_around_send5 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงวันที่ 3");
      check_around_send5 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 7) && (check_around_send6 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงวันที่ 7");
      check_around_send6 = true;
    }

    //// DAY_NOTIFY_RED_FULL_HIGH
    if ((DAY_NOTIFY_RED_FULL_HIGH == 1) && (check_around_send7 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงมากวันที่ 1");
      check_around_send7 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 3) && (check_around_send8 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงมากวันที่ 3 คำแนะนำ :เตรียมสูบน้ำออกจากนา");
      check_around_send8 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 7) && (check_around_send9 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงมากวันที่ 7 คำแนะนำ :ควรสูบน้ำออกจากนา");
      check_around_send9 = true;
    }

    if (Rice_age_start_count == 20) {
      DAY_NOTIFY_RED_FULL_LOW = 0;
      DAY_NOTIFY_YELLOW_MINI_LOW = 0;
      DAY_NOTIFY_GREEN_NORMAL = 0;
      DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
      DAY_NOTIFY_RED_FULL_HIGH = 0;
    }
  }

  //case 21 - 99 day use water 5-10 cm
  if ((Rice_age_start_count >= 21 ) && (Rice_age_start_count <= 99)) {
    if ((cm >= 0) && (cm <= 20)) {
      CALL_RED_FULL_LOW();
    }
    if ((cm > 20) && (cm <= 50)) {
      CALL_YELLOW_MINI_LOW();
    }
    if ((cm > 50) && (cm <= 100)) {
      CALL_GREEN_NORMAL();
    }
    if ((cm > 100) && (cm <= 150)) {
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 150) {
      CALL_RED_FULL_HIGH();
    }
    if ((RED_FULL_LOW == true) && (check_around1 == false)) {
      DAY_NOTIFY_RED_FULL_LOW += 1;
      Serial.println("RED_FULL_LOW");
      if (DAY_NOTIFY_RED_FULL_LOW == 1) {
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        check_around_send1 = false;
        check_around_send2 = false;
        check_around_send3 = false;
      }
      check_around1 = true;
    }
    if ((YELLOW_MINI_LOW == true) && (check_around2 == false)) {
      DAY_NOTIFY_YELLOW_MINI_LOW += 1;
      Serial.println("YELLOW_MINI_LOW");
      if (DAY_NOTIFY_YELLOW_MINI_LOW == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        check_around_send4 = false;
        check_around_send5 = false;
        check_around_send6 = false;
      }
      check_around2 = true;
    }
    if ((GREEN_NORMAL == true) && (check_around3 == false)) {
      DAY_NOTIFY_GREEN_NORMAL += 1;
      Serial.println("GREEN_NORMAL");
      if (DAY_NOTIFY_GREEN_NORMAL == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        check_around_send7 = false;
        check_around_send8 = false;
        check_around_send9 = false;
      }
      check_around3 = true;
    }
    if ((YELLOW_MINI_HIGH == true) && (check_around4 == false)) {
      DAY_NOTIFY_YELLOW_MINI_HIGH += 1;
      Serial.println("YELLOW_MINI_HIGH");
      if (DAY_NOTIFY_YELLOW_MINI_HIGH == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        check_around_send10 = false;
        check_around_send11 = false;
        check_around_send12 = false;
      }
      check_around4 = true;
    }
    if ((RED_FULL_HIGH == true) && (check_around5 == false)) {
      DAY_NOTIFY_RED_FULL_HIGH += 1;
      Serial.println("RED_FULL_HIGH");
      if (DAY_NOTIFY_RED_FULL_HIGH == 1) {
        DAY_NOTIFY_RED_FULL_LOW = 0;
        DAY_NOTIFY_YELLOW_MINI_LOW = 0;
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        check_around_send13 = false;
        check_around_send14 = false;
        check_around_send15 = false;
      }
      check_around5 = true;
    }

    //// DAY_NOTIFY_RED_FULL_LOW
    if ((DAY_NOTIFY_RED_FULL_LOW == 1) && (check_around_send1 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์ต่ำมากวันที่ 1 ");
      check_around_send1 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_LOW == 3) && (check_around_send2 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์ต่ำมากวันที่ 3 คำแนะนำ : เตรียมสูบน้ำเข้านา");
      check_around_send2 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_LOW == 7) && (check_around_send3 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์ต่ำมากวันที่ 7 คำแนะนำ : ควรสูบน้ำเข้านา");
      check_around_send3 = true;
    }

    //// DAY_NOTIFY_YELLOW_MINI_LOW
    if ((DAY_NOTIFY_YELLOW_MINI_LOW == 1) && (check_around_send4 == false)) {
      // LINE.notify("ระดับน้ำอยู่ในเกณฑ์ต่ำ!! วันแรก");
      check_around_send4 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_LOW == 3) && (check_around_send5 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์ต่ำวันที่ 3");
      check_around_send5 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_LOW == 7) && (check_around_send6 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์ต่ำวันที่ 7 ");
      check_around_send6 = true;
    }

    //// DAY_NOTIFY_GREEN_NORMAL
    if ((DAY_NOTIFY_GREEN_NORMAL == 1) && (check_around_send7 == false)) {
      // LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! วันแรก");
      check_around_send7 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 7) && (check_around_send8 == false)) {
      // LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 7 วันแล้ว ");
      check_around_send8 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 14) && (check_around_send9 == false)) {
      // LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 14 วันแล้ว");
      check_around_send9 = true;
    }

    //// DAY_NOTIFY_YELLOW_MINI_HIGH
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 1) && (check_around_send10 == false)) {
      //LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! วันแรก");
      check_around_send10 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 3) && (check_around_send11 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงวันที่ 3");
      check_around_send11 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 7) && (check_around_send12 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงวันที่ 7");
      check_around_send12 = true;
    }

    //// DAY_NOTIFY_RED_FULL_HIGH
    if ((DAY_NOTIFY_RED_FULL_HIGH == 1) && (check_around_send13 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงมากวันที่ 1");
      check_around_send13 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 3) && (check_around_send14 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงมากวันที่ 3 คำแนะนำ :เตรียมสูบน้ำออกจากนา");
      check_around_send14 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 7) && (check_around_send15 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงมากวันที่ 7 คำแนะนำ :ควรสูบน้ำออกจากนา");
      check_around_send15 = true;
    }

    if (Rice_age_start_count == 99) {
      DAY_NOTIFY_RED_FULL_LOW = 0;
      DAY_NOTIFY_YELLOW_MINI_LOW = 0;
      DAY_NOTIFY_GREEN_NORMAL = 0;
      DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
      DAY_NOTIFY_RED_FULL_HIGH = 0;
    }
  }

  //case 100 -120 day  no water 0 cm
  if (Rice_age_start_count >= 100)
    //&& (Rice_age_start_count <= 120))
  {
    if ((cm >= 0) && (cm <= 20)) {
      CALL_GREEN_NORMAL();
    }
    if ((cm > 20) && (cm <= 50)) {
      CALL_YELLOW_MINI_HIGH();
    }
    if (cm > 50) {
      CALL_RED_FULL_HIGH();
    }
    if ((GREEN_NORMAL == true) && (check_around3 == false)) {
      DAY_NOTIFY_GREEN_NORMAL += 1;
      Serial.println("GREEN_NORMAL");
      if (DAY_NOTIFY_GREEN_NORMAL == 1) {
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        check_around_send1 = false;
        check_around_send2 = false;
        check_around_send3 = false;
      }
      check_around3 = true;
    }
    if ((YELLOW_MINI_HIGH == true) && (check_around4 == false)) {
      DAY_NOTIFY_YELLOW_MINI_HIGH += 1;
      Serial.println("YELLOW_MINI_HIGH");
      if (DAY_NOTIFY_YELLOW_MINI_HIGH == 1) {
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_RED_FULL_HIGH = 0;
        check_around_send4 = false;
        check_around_send5 = false;
        check_around_send6 = false;
      }
      check_around4 = true;
    }
    if ((RED_FULL_HIGH == true) && (check_around5 == false)) {
      DAY_NOTIFY_RED_FULL_HIGH += 1;
      Serial.println("RED_FULL_HIGH");
      if (DAY_NOTIFY_RED_FULL_HIGH == 1) {
        DAY_NOTIFY_GREEN_NORMAL = 0;
        DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
        check_around_send7 = false;
        check_around_send8 = false;
        check_around_send9 = false;
      }
      check_around5 = true;
    }

    //// DAY_NOTIFY_GREEN_NORMAL
    if ((DAY_NOTIFY_GREEN_NORMAL == 1) && (check_around_send1 == false)) {
      //LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! วันแรก");
      check_around_send1 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 3) && (check_around_send2 == false)) {
      //LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 7 วันแล้ว");
      check_around_send2 = true;
    }
    if ((DAY_NOTIFY_GREEN_NORMAL == 7) && (check_around_send3 == false)) {
      //LINE.notify("ระดับน้ำอยู่ในเกณฑ์ปกติ!! 14 วันแล้ว");
      check_around_send3 = true;
    }

    //// DAY_NOTIFY_YELLOW_MINI_HIGH
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 1) && (check_around_send4 == false)) {
      //LINE.notify("ระดับน้ำอยู่ในเกณฑ์สูง!! วันแรก");
      check_around_send4 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 3) && (check_around_send5 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงมาวันที่ 3");
      check_around_send5 = true;
    }
    if ((DAY_NOTIFY_YELLOW_MINI_HIGH == 7) && (check_around_send6 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงมาวันที่ 7");
      check_around_send6 = true;
    }

    //// DAY_NOTIFY_RED_FULL_HIGH
    if ((DAY_NOTIFY_RED_FULL_HIGH == 1) && (check_around_send7 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงมากวันที่ 1");
      check_around_send7 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 3) && (check_around_send8 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงมากวันที่ 3 คำแนะนำ :เตรียมสูบน้ำออกจากนา");
      check_around_send8 = true;
    }
    if ((DAY_NOTIFY_RED_FULL_HIGH == 7) && (check_around_send9 == false)) {
      LINE.notify("อายุข้าวปัจจุบัน " + String(Rice_age_start_count) + " วัน ความสูงของน้ำ " + String(cm) + " มิลลิเมตร  ระดับน้ำอยู่ในเกณฑ์สูงมากวันที่ 7 คำแนะนำ :ควรสูบน้ำออกจากนา");
      check_around_send9 = true;
    }

    if (Rice_age_start_count == 120) {
      DAY_NOTIFY_RED_FULL_LOW = 0;
      DAY_NOTIFY_YELLOW_MINI_LOW = 0;
      DAY_NOTIFY_GREEN_NORMAL = 0;
      DAY_NOTIFY_YELLOW_MINI_HIGH = 0;
      DAY_NOTIFY_RED_FULL_HIGH = 0;
    }
  }
}
