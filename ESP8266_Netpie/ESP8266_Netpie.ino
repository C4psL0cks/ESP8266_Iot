#include <ESP8266WiFi.h>
// Netpie
#include <MicroGear.h>
// JSON
#include <ArduinoJson.h>
// LINE
#include <TridentTD_LineNotify.h>
#include <Wire.h>
#include <time.h>
// OLED
#include <ACROBOTIC_SSD1306.h>
// wifi
#define SSID_WIFI    "AndroidAPD261"
#define PASSWORD_WIFI   "mojijosh"
// Line
#define LINE_TOKEN  "0lbZcis2QugRyJv6SZaxzcK2STZOkzWqD7TnDqegFbM"
// Netpie
#define APPID "smartbins"
#define KEY "4gb6nh27xR6cc6W"
#define SECRET "6QIrz3dXFOBNbp4P3hVX6SulZ"
#define ALIAS "esp8266"
#define FEEDID "BINSFEEDS1"
#define INTERVAL 15000
#define T_INCREMENT 200
#define T_RECONNECT 5000
#define BAUD_RATE 115200
#define MAX_TEMP 100
#define MAX_HUMID 100

// sensor Ultrasonic
#define TRIGGER_PIN  D5
#define ECHO_PIN     D6

WiFiClient client;

// ตัวแปลสำหรับ sensor ทั้งสองตัว
int timer = 0;
char str[32];
long duration, distance;
float temp;
int Read;
float Val;

/// time NTP server
struct tm* p_tm;
char ntp_server1[20] = "ntp.ku.ac.th";
char ntp_server2[20] = "fw.eng.ku.ac.th";
char ntp_server3[20] = "time.uni.net.th";
int sec;
String times = "";
// check รอบการส่งไลน์
bool checksend = false;

MicroGear microgear(client);
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message -- >"); msg[msglen] = '\0';
  Serial.println((char *)msg);
}
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
}
void setup() {

  microgear.on(MESSAGE, onMsghandler);
  microgear.on(CONNECTED, onConnected);
  Serial.begin(115200);
  Wire.begin();
  oled.init();                      // Initialze SSD1306 OLED display
  oled.clearDisplay();              // Clear screen
  Serial.println("Starting...");
  oled.setTextXY(1, 0);             // Set cursor position, start of line 1
  oled.putString("Starting...");
  // ส่วนของการเชื่อม wifi
  WiFi.begin(SSID_WIFI, PASSWORD_WIFI);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // เชื่อมต่อ line notify
  LINE.setToken(LINE_TOKEN);
  // ส่วนของการเชื่อมต่อ NTP server
  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  LINE.notify("เชื่อมต่อเรียบร้อย....");
  // clear  จอ
  oled.clearDisplay();
  // netpie
  microgear.init(KEY, SECRET, ALIAS);
  microgear.connect(APPID);
}
void loop() {

  if (microgear.connected()) {
    Serial.println("connected");
    microgear.loop();

    time_t time_now = time(nullptr);   // Time
    p_tm = localtime(&time_now);  // ตัวแปล time
    sec = p_tm->tm_min; //tm_min // เวลาสปัจุบัญที่เป็นหน่วย นาที เอาใช้สำหรับเช็คการส่ง line
    times = ctime(&time_now);  // เวลาปัจุบัญที่ได้มาจาก NTP server

    DynamicJsonBuffer jsonBuffer;   // กำหนด jsonbuffer แบบ Dynamic(ไม่กำหนดตายตัว ขยายได้เรือยๆ)
    JsonObject& root = jsonBuffer.createObject(); // สร้าง object มารอรับค่าที่ชื่อว่า root[]

    // ส่วนของ sensor voltage
    Read = analogRead(A0); // อ่านค่าจาก senoser มาที่ขา A0 แบบ Analog
    temp = Read / 5.092;
    Read = (int)temp;
    Val = ((Read % 100) / 10.0);  // ค่าที่ได้จากการคำนวนของ sensor
    //Serial.println("Voltage:" + String(Val));
    oled.setTextXY(2, 0);             // Set cursor position, start of line 1
    oled.putString("Volt:" + String(Val));

    // ส่วนของ sensor Ultrasonic
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;
    if (distance > 30) { // ปกติถังลึก 30 cm ถ้า sensor อ่านได้ เกิน 30 แสดงว่าเกิดค่าที่ error ให้ set ค่ากลับไปเป็น 0
      distance = 0; // set ให้ = 0
    }
    //Serial.println(distance);
    oled.setTextXY(3, 0);             // Set cursor position, start of line 1
    oled.putString("CM:" + String(distance));

    if ((sec % 5) == 0) {
      Serial.println("CHECK:" + String(sec));
      // ถ้า sec ก็คือ หน่วยนาที มันหาร 5 ลงตัว มันจะเข้าเคสนี้ละเช็คต่อว่า ขยะ เต็มมั้ยถ้าเต็ม มันจะส่งไปที่ไลน์ แค่ครั่งเดียว
      if (distance < 10 && checksend == false) { // เช็คว่า ขยะเต็มมั้ยถ้าเต็ม
        LINE.notify("ถังขยะเต็ม"); // ส่งไปที่ line
        Serial.println("Full");
        checksend = true;
      }
    }
    else { // ถ้าไม่เข้าเคส ข้างบน ให้ reset ตัวแปลการส่ง 1 ครั้ง ให้สามารถส่งใน 5 นาทีถัดไปได้
      //Serial.println("RESRT:" + String(sec));
      checksend = false;
    }

    root["bin"] = distance;  // เอาค่าตัวแปลของ ถังขยะ มาเก็บใส่ json
    root["voltage"] = Val;  // เอาค่าตัวแปลของ แบต มาเก็บใส่ json
    String jsonData;
    root.printTo(jsonData); // จับยัดเข้า object json
    //Serial.println(jsonData);

    if (timer >= 1000) { // ทุกๆ 1 วิจะทำการส่งค่าไป netpie
      sprintf(str, "%d,%.2f", distance, Val);  // แสดงค่าที่ได้รับมาจากด้านบน
      Serial.println(str);
      Serial.print("Sending -- >");
      microgear.publish("/sensor", str);  // ส่งค่าไปยัง netpie ที่ path/sensor
      microgear.writeFeed(FEEDID, jsonData); // ส่งไปแบบ json
      timer = 0;
    } else {
      timer += 100;
    }
  }
  //=========================ถ้าต่อไม่ติด==========================
  else {
    Serial.println("connection lost, reconnect...");
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
    }
    else timer += 100;
  }
  delay(300);
}
