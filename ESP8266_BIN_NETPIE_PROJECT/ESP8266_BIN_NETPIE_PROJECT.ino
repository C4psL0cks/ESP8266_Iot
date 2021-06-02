#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include <ArduinoJson.h>
#include <TridentTD_LineNotify.h>
#include <Wire.h>
#include <time.h>
#include <ACROBOTIC_SSD1306.h>
#define WIFI_SSID   "dlink-ABD0"
#define WIFI_PASS   "yyafr68490"
#define LINE_TOKEN  "0lbZcis2QugRyJv6SZaxzcK2STZOkzWqD7TnDqegFbM"
#define APPID "smartbins"
#define KEY "4gb6nh27xR6cc6W"
#define SECRET "6QIrz3dXFOBNbp4P3hVX6SulZ"
#define ALIAS "esp8266"
#define FEEDID "BINSFEEDS1"

#define TRIG 14
#define ECHO 12

WiFiClient client;
//
float vout = 0.0;
float vin = 0.0;
float R1 = 30000;
float R2 = 7500;
int value = 0;

//
long duration, distance;
int cm = 0, bincm = 30;
int send_cm = 0, send_vin = 0;

//
int timer = 0;
char str[32];

/// time NTP server
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
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // OLED
  Wire.begin();
  oled.init();
  oled.clearDisplay();
  oled.setTextXY(1, 1);
  oled.putString("$$SMART BIN$$");

  // Connect to Wifi.
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("$$SMART BIN$$");
  Serial.println("-------------------------------------");
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

  //Line notify
  LINE.setToken(LINE_TOKEN);

  //NTP server
  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  LINE.notify("เชื่อมต่อเรียบร้อย....");

  //netpie
  microgear.init(KEY, SECRET, ALIAS);
  microgear.connect(APPID);
}
void loop() {

  if (microgear.connected()) {
    microgear.loop();

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    time_t time_now = time(nullptr);
    p_tm = localtime(&time_now);
    sec = p_tm->tm_min;
    times = ctime(&time_now);

    //Voltage sensor
    value = analogRead(A0);
    vout = (value * 3.0) / 1023.0;
    vin = vout / (R2 / (R1 + R2));
    if (vin < 0 ) {
      vin = 0;
    }
    //Serial.println("Voltage:" + String(vin));
    // เข้าสูตร
    send_vin = (vin * 100) / 5;
    //Serial.println("Voltage:" + String(send_vin));

    oled.setTextXY(2, 0);
    oled.putString("Volt:" + String(send_vin));
    oled.setTextXY(2, 10);
    oled.putString("%");

    //Ultrasonic sensor
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    duration = pulseIn(ECHO, HIGH);
    distance = (duration / 2) / 29.1;
    cm = (bincm - distance);
    if (cm < 0 ) {
      cm = 0;
    }
    //Serial.println("distance:" + String(cm));
    // เข้าสูตร
    send_cm = (cm * 100) / 30;
    //Serial.println("send_cm:" + String(send_cm));

    oled.setTextXY(3, 0);
    oled.putString("CM:" + String(send_cm));
    oled.setTextXY(3, 10);
    oled.putString("%");

    if ((sec % 5) == 0) {
      //Serial.println(Time:" + String(sec));
      if (send_cm > 70 && checksend == false) {
        LINE.notify("ถังขยะเต็ม");
        checksend = true;
      }
    }
    else {
      checksend = false;
    }

    if (timer >= 500) {
      root["bin"] = send_cm;
      root["voltage"] = send_vin;
      String jsonData;
      root.printTo(jsonData);
      sprintf(str, "%d,%d", send_cm, send_vin);
      Serial.println(str);
      Serial.print("Sending -- >");
      microgear.publish("/sensor", str);
      microgear.writeFeed(FEEDID, jsonData);
      timer = 0;
    } else {
      timer += 100;
    }
  }
  else {
    Serial.println("connection lost, reconnect...");
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
    }
    else timer += 100;
  }
  delay(150);
}
