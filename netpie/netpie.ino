#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include <ArduinoJson.h>
#include <TridentTD_LineNotify.h>
#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>
const char* ssid     = "6021607";
const char* password = "17401449";
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
int Val;


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
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    oled.setTextXY(2, 0);             // Set cursor position, start of line 2
    oled.putString(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  LINE.setToken(LINE_TOKEN);
  LINE.notify("เชื่อมต่อเรียบร้อย....");
  oled.setTextXY(2, 0);             // Set cursor position, start of line 2
  oled.putString("WiFi connected");
  oled.setTextXY(3, 0);             // Set cursor position, start of line 3
  oled.putString("IP address: ");
  oled.setTextXY(4, 0);             // Set cursor position, start of line 4
  oled.putString(WiFi.localIP().toString());
  //delay(1500);
  oled.clearDisplay();              // Clear screen

  // netpie
  microgear.init(KEY, SECRET, ALIAS);
  microgear.connect(APPID);
}
void loop() {

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  Read = analogRead(A0);
  temp = Read / 4.092;
  Read = (int)temp;
  Val = ((Read % 100) / 10.0);
  Serial.println("Voltage:" + String(Val));
  oled.setTextXY(2, 0);
  oled.putString("Voltage:" + String(Val));

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW);
  //delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  //delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1;
  Serial.print(distance);
  oled.setTextXY(3, 0);             
  oled.putString("Distance:" + String(distance));
  //Serial.println(" cm");
  //  if (distance <= 15) {
  //    LINE.notify("ถังขยะเต็ม");
  //  }
  root["bin"] = distance;
  root["voltage"] = Val;
  String jsonData;
  root.printTo(jsonData);
  //Serial.println(jsonData);

  if (microgear.connected()) {
    Serial.println("connected");
    microgear.loop();

    if (timer >= 1000) {
      sprintf(str, "%d,%d", distance, Val);
      Serial.println(str);
      Serial.print("Sending -- >");
      microgear.publish("/sensor", str);
      microgear.writeFeed(FEEDID, jsonData);
      timer = 0;
    } else {
      timer += 100;
    }
  } else {
    Serial.println("connection lost, reconnect...");
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
    }
    else timer += 100;
  }
  delay(100);
}
