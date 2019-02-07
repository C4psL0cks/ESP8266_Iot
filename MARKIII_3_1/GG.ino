#include <Wire.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TridentTD_LineNotify.h>
#define BLYNK_PRINT Serial
#define BLYNK_MAX_SENDBYTES 256

#define BLYNK_TOKEN   "34b0f27c2a7d4c7db6bcb0c11b9ea7e6"
#define LINE_TOKEN    "wiN66datagETOi1HG1UZIqWLeOODgBg0u0dXtCDKe4C"
#define WIFI_SSID     "6021607"
#define WIFI_PASS     "17401449"

#define CLOSED        0
#define OPENED        1
#define VT_PIN        A0
#define AT_PIN        A0
#define LEDPINRED     D5
#define LEDPINBLUE    D6
#define RELAYPIN      D1
#define MAGNETICPIN   D2
#define SWITCHPIN     D0

bool BLYNK_ENABLED = true;
bool CHECKCLOSES = false;
bool CHECKOPENS = false;
bool STATUSDOOR = false;   // ปิดไว้ตอนเริ่มต้น รอตามรีเรีย์
bool STATUSRELAY = false;  // ปิดไว้ตอนเริ่มต้น รอสั่ง
bool STATUSSWITCH = false; // ปิดไว้ตอนเริ่มต้น รอกด
double voltage, current;
int vt_temp, at_temp;
int BT1, BT2;
int DOORSTATE = 0;
int state = 0;
WidgetLED LED_RED_OPEN(V3);   // open
WidgetLED LED_BLUE_CLOSE(V4); // close red
WidgetLED LED_ALERT(V8); // close red
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
  Blynk.syncAll();
}
BLYNK_WRITE(V0) {
  BT1 = param.asInt();
  if (BT1 == 0) {
    CHECKCLOSES = true;
  }
  if (BT1 == 1) {
    CHECKOPENS = true;
  }
}
BLYNK_WRITE(V7) {
  BT2 = param.asInt();
  if (BT2 == 0) {
    CHECKCLOSES = true;
  }
  if (BT2 == 1) {
    CHECKOPENS = true;
  }
}
void RELAYCLOSE() {
  STATUSRELAY = false;
  digitalWrite(RELAYPIN, LOW);    // ทำงาน lock
}
void RELAYOPEN() {
  STATUSRELAY = true;
  digitalWrite(RELAYPIN, HIGH);  // ไม่ทำงาน unlock
}
void SWITCH() {
  if (digitalRead(SWITCHPIN) == LOW) {
    STATUSSWITCH = false; // ถูกปล่อย
  }
  if (digitalRead(SWITCHPIN) == HIGH) {
    STATUSSWITCH = true;  // ถูกกด
  }
}
void DOOR() {
  if (digitalRead(MAGNETICPIN) == HIGH && DOORSTATE == OPENED) {
    DOORSTATE = CLOSED;
    STATUSDOOR = false; // ปิด
    Serial.println("CLOSE");
  }
  if (digitalRead(MAGNETICPIN) == LOW && DOORSTATE == CLOSED) {
    DOORSTATE = OPENED;
    STATUSDOOR = true;  // เปิด
    Serial.println("OPEN");
  }
}
void setup() {

  Serial.begin(115200);
  pinMode(LEDPINRED, OUTPUT);   digitalWrite(LEDPINRED, HIGH);
  pinMode(LEDPINBLUE, OUTPUT);  digitalWrite(LEDPINBLUE, HIGH);
  pinMode(RELAYPIN, OUTPUT);    digitalWrite(RELAYPIN, LOW);
  pinMode(MAGNETICPIN, INPUT);
  pinMode(SWITCHPIN, INPUT);    digitalWrite(SWITCHPIN, LOW); // low wait high

  LED_RED_OPEN.off();   // open
  LED_BLUE_CLOSE.off(); // close
  LED_ALERT.off();

  if (strlen(BLYNK_TOKEN) == 0) {
    BLYNK_ENABLED = false;
  }
  if (BLYNK_ENABLED) {
    Blynk.begin(BLYNK_TOKEN, WIFI_SSID, WIFI_PASS);
    LINE.setToken(LINE_TOKEN);
    LINE.notify("Connect...Online :)");
  }
}
void loop() {

  if (BLYNK_ENABLED) {
    Blynk.run();
  }
  if (!Blynk.connected()) {
    Blynk.connect();
  }
  DOOR();
  SWITCH();
  Blynk.virtualWrite(V1, STATUSDOOR);  // MAGNETIC
  Blynk.virtualWrite(V2, STATUSRELAY); // RELAY
  vt_temp = analogRead(VT_PIN); voltage = vt_temp * (3.40 / 1023.0) * 5;
  at_temp = analogRead(AT_PIN); current = at_temp * (3.40 / 1023.0);
  Blynk.virtualWrite(V5, voltage);
  Blynk.virtualWrite(V6, current);

  if ((STATUSDOOR == false) && (STATUSRELAY == false)) {
    LED_ALERT.off();
  }
  if ((STATUSDOOR == true) && (STATUSRELAY == false)) {
    //BUZZERON();
    RELAYCLOSE();
    LED_ALERT.on();
    LED_BLUE_CLOSE.off();
    Blynk.virtualWrite(V0, STATUSRELAY); // RELAY
    Blynk.virtualWrite(V1, STATUSDOOR); // RELAY
    digitalWrite(LEDPINRED, HIGH);        // แดงดับ
    //LINE.notify("Attack the door!!");
    //Blynk.notify("Attack the door!!");
    Serial.println("Attack the door!!");
  }
  if (BT1 == 0 && CHECKCLOSES == true) {
    RELAYCLOSE();
    LED_RED_OPEN.off();
    LED_BLUE_CLOSE.on();
    Blynk.virtualWrite(V0, STATUSRELAY); // RELAY
    digitalWrite(LEDPINRED, LOW);    // แดงดับ
    digitalWrite(LEDPINBLUE, HIGH);  // น้ำเงินติด
    Serial.println("DOOR CLOSE");
    //LINE.notify("DOOR CLOSE");
    //Blynk.notify("DOOR CLOSE");
    CHECKCLOSES = false;
  }
  if (BT1 == 1 && CHECKOPENS == true) {
    RELAYOPEN();
    LED_RED_OPEN.on();
    LED_BLUE_CLOSE.off();
    Blynk.virtualWrite(V0, STATUSRELAY); // RELAY
    digitalWrite(LEDPINBLUE, HIGH);  // น้ำเงินติด
    digitalWrite(LEDPINRED, LOW);    // แดงดับ
    Serial.println("DOOR OPEN");
    //LINE.notify("DOOR OPEN");
    //Blynk.notify("DOOR OPEN");
    CHECKOPENS = false;
  }
  if (BT2 == 0 && CHECKCLOSES == true) {
    RELAYCLOSE();
    LED_RED_OPEN.off();
    LED_BLUE_CLOSE.on();
    Blynk.virtualWrite(V0, STATUSRELAY); // RELAY
    digitalWrite(LEDPINRED, LOW);    // แดงดับ
    digitalWrite(LEDPINBLUE, HIGH);  // น้ำเงินติด
    Serial.println("DOOR CLOSE");
    //LINE.notify("DOOR CLOSE");
    //Blynk.notify("DOOR CLOSE");
    CHECKCLOSES = false;
  }
  if (BT2 == 1 && CHECKOPENS == true) {
    RELAYOPEN();
    LED_RED_OPEN.on();
    LED_BLUE_CLOSE.off();
    Blynk.virtualWrite(V0, STATUSRELAY); // RELAY
    digitalWrite(LEDPINBLUE, HIGH);  // น้ำเงินติด
    digitalWrite(LEDPINRED, LOW);    // แดงดับ
    Serial.println("DOOR OPEN");
    //LINE.notify("DOOR OPEN");
    //Blynk.notify("DOOR OPEN");
    CHECKOPENS = false;
  }
  if (STATUSSWITCH) {
    RELAYOPEN();
    LED_RED_OPEN.on();
    LED_BLUE_CLOSE.off();
    Blynk.virtualWrite(V0, STATUSRELAY); // RELAY
    digitalWrite(LEDPINBLUE, HIGH);  // น้ำเงินติด
    digitalWrite(LEDPINRED, LOW);    // แดงดับ
    Serial.println("DOOR OPEN");
    //LINE.notify("DOOR OPEN");
    //Blynk.notify("DOOR OPEN");
    delay(1000);
    RELAYCLOSE();
    LED_RED_OPEN.off();
    LED_BLUE_CLOSE.on();
    Blynk.virtualWrite(V0, STATUSRELAY); // RELAY
    digitalWrite(LEDPINRED, LOW);    // แดงดับ
    digitalWrite(LEDPINBLUE, HIGH);  // น้ำเงินติด
    Serial.println("DOOR CLOSE");
    //LINE.notify("DOOR CLOSE");
    //Blynk.notify("DOOR CLOSE");
    CHECKOPENS = false;
    CHECKCLOSES = false;
  }
}
