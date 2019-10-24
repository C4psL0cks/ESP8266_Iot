#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include <ArduinoJson.h>
#include <TridentTD_LineNotify.h>
#include <Wire.h>
#include <time.h>
#include <ACROBOTIC_SSD1306.h>
#define SSID_WIFI    "AndroidAPD261"
#define PASSWORD_WIFI   "mojijosh"
#define LINE_TOKEN  "0lbZcis2QugRyJv6SZaxzcK2STZOkzWqD7TnDqegFbM"
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
// sensor
#define TRIGGER_PIN  D5
#define ECHO_PIN     D6

WiFiClient client;

int timer = 0;
char str[32];
long duration, distance;
float temp;
int Read;
float Val;

/// time
struct tm* p_tm;
char ntp_server1[20] = "ntp.ku.ac.th";
char ntp_server2[20] = "fw.eng.ku.ac.th";
char ntp_server3[20] = "time.uni.net.th";
int sec;
String times = "";
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
  WiFi.begin(SSID_WIFI, PASSWORD_WIFI);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  LINE.setToken(LINE_TOKEN);
  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  LINE.notify("เชื่อมต่อเรียบร้อย....");
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
    p_tm = localtime(&time_now);
    sec = p_tm->tm_min; //tm_min
    times = ctime(&time_now);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    Read = analogRead(A0);
    temp = Read / 5.092;
    Read = (int)temp;
    Val = ((Read % 100) / 10.0);
    //Serial.println("Voltage:" + String(Val));

    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;
    if (distance > 30) {
      distance = 0;
    }
    //Serial.println(distance);

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

    root["bin"] = distance;
    root["voltage"] = Val;
    String jsonData;
    root.printTo(jsonData);
    //Serial.println(jsonData);

    if (timer >= 1000) {
      sprintf(str, "%d,%.2f", distance, Val);
      Serial.println(str);
      Serial.print("Sending -- >");
      microgear.publish("/sensor", str);
      microgear.writeFeed(FEEDID, jsonData);
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
