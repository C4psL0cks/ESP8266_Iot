
//เรียกใช้ libary times
#include <time.h>
//เรียกใช้ libary esp8266 board
#include <ESP8266WiFi.h>
//เรียกใช้ libary LineNotify
#include <TridentTD_LineNotify.h>

//เรียกใช้ libary การใช้จอ OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET     -1
Adafruit_SSD1306 display(OLED_RESET);


//กำหนดค่า wifi password linetoken sensorpin
#define WIFI_SSID   "6021607"
#define WIFI_PASS   "17401449"
#define LINE_TOKEN  "VOwca6uoY6T5vR6A43J7zwg2PtOWG2z4wGvgTLFsN3u"
#define sensor1 14
#define sensor2 12
#define sensor5 13
#define sensor10 15

// กำหนดตัวแปลสำหรับเวลา
struct tm* p_tm;
char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";
String times = "";
int Minutes;
int Hours;
int sum;

// กำหนดจตัวแปลสำหรับการเช็ค และการนับเหรียญ
bool check_reset = false;
bool isCounter1 = false, isCounter2 = false, isCounter5 = false, isCounter10 = false;
int count1 = 0, count2 = 0, count5 = 0, count10 = 0;

// ฟังชั่น setup
void setup() {

  // กำหนด serialport
  Serial.begin(115200);

  // เช็คการเชื่อมต่อจอ
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // ระบุชนิดของการต่อ sensor ว่า input หรือ output
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor5, INPUT);
  pinMode(sensor10, INPUT);

  // คำสั่ง interup เมื่อเกิดการงานจะกระโดดการทำเข้ามาทำงานที่ตรงนี้ทันที
  attachInterrupt(sensor1, doCounter1, CHANGE);
  attachInterrupt(sensor2, doCounter2, CHANGE);
  attachInterrupt(sensor5, doCounter5, CHANGE);
  attachInterrupt(sensor10, doCounter10, CHANGE);

  // การเชื่อมต่อไวฟาย
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("Running!");
  Serial.println("-------------------------------------");
  // Connect to Wifi.
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("WiFi Connecting to %s\n", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // เซต token line
  LINE.setToken(LINE_TOKEN);

  // เซต เวลาที่เชื่อมต่อ
  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3); // connect ntp server clock
  Serial.println("\nWaiting for time");
  LINE.notify("รอการเชื่อมต่อเวลาสักครู่...สำเร็จแล้ว");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }

}

// ฟังชั่น loop
void loop() {

  // เคลียร์จอ
  display.clearDisplay();

  // ตัวแปลเวลาที่รับค่าเวลาปัจุบัน
  time_t time_now = time(nullptr);
  p_tm = localtime(&time_now);
  Minutes = p_tm->tm_min;
  Hours = p_tm->tm_hour;
  times = ctime(&time_now);
  Serial.println("TIME NOW:" + String(times));

  // นับเหรียญ 1 บาท
  if (isCounter1 == true) {
    isCounter1 = false;
    count1 += 1;
    Serial.println("count 1 :" + String(count1));
  }

  // นับเหรียญ 2 บาท
  if (isCounter2 == true) {
    isCounter2 = false;
    count2 += 2;
    Serial.println("count 2 :" + String(count2));
  }

  // นับเหรียญ 5 บาท
  if (isCounter5 == true) {
    isCounter5 = false;
    count5 += 5;
    Serial.println("count 5 :" + String(count5));
  }

  // นับเหรียญ 10 บาท
  if (isCounter10 == true) {
    isCounter10 = false;
    count10 += 10;
    Serial.println("count 10 :" + String(count10));
  }

  // รวมจำนวนของเหรียญ
  sum = count1 + count2 + count5 + count10;
  Serial.println(sum);
  
  // แสดงผลจำนวนเหรียญบนหน้าจอ
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Coin 1 :" + String(count1));
  display.setCursor(0, 8);
  display.println("Coin 2 :" + String(count2));
  display.setCursor(0, 16);
  display.println("Coin 5 :" + String(count5));
  display.setCursor(0, 24);
  display.println("Coin 10 :" + String(count10));
  display.display();

  // แสดงเวลาในหน่วย ชั่วโมง
  Serial.println("Hours:" + String(Hours));
  if (Hours == 20) {
    // เมื่อเวลาเท่ากับ 20.00 จะทำการส่งข้อความไปยังไลน์
    if (Hours == 20 && check_reset == false) {
      Serial.println("CHECK:" + String(Hours));
      LINE.notify("\nเหรียญ 1 : " + String(count1) + " ฿\nเหรียญ 2 : " + String(count2) + " ฿\nเหรียญ 5 : " + String(count5) + " ฿\nเหรียญ 10 : " + String(count10) + " ฿\nเหรียญทั้งหมด : " + String(sum) + " ฿");
      check_reset = true;
    }
  }
  // เมื่อไม่อยู่ในขอบเขตเวลา 20.00 จะทำการรีเซตตัวแปลสำหรับการส่งข้อความไปหาไลน์
  else {
    Serial.println("RESRT:" + String(Hours));
    check_reset = false;
  }
  // หน่วงเวลาการทำงานทุกๆครั้งต่อ 1 วินาที
  delay(1000);
}

void doCounter1() {
  isCounter1 = true;
}
void doCounter2() {
  isCounter2 = true;
}
void doCounter5() {
  isCounter5 = true;
}
void doCounter10() {
  isCounter10 = true;
}
