void Line_Notify(String message) ;
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "DHT.h"
#define DHTPIN D4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define WIFI_SSID "A1"
#define WIFI_PASSWORD "asdf"

#define LINE_TOKEN "o6XCVFsZPvqhL4qNykWUcs8W7z4fMwjfa1gRdrB9MEi"
#define SW1 D2
#define SW2 D3

unsigned long previousMillis = 0;
const long interval = 21600000;

int buttonState1 = 0;
int buttonState2 = 0;  //เพิ่ม channel2
int Cout1 = 0;
int Cout2 = 0;         //เพิ่ม channel2
void setup() {

  Serial.begin(115200);
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println(WIFI_SSID);
  Serial.print("connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WIFI_SSID);
  Serial.println(WiFi.localIP());
  String message = "WiFi connect to finish!!" + WiFi.localIP(); //การเชื่อมต่อ WiFi
  Line_Notify(message);
}
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    String message = "TEST3";
    Line_Notify(message);
  }
  buttonState1 = digitalRead(SW1);
  buttonState2 = digitalRead(SW2);  //เพิ่ม channel2
  // เริ่มต้น ----------------------- ช่องที่ 1 D2 ---------------------------
  if (buttonState1 == HIGH) {
    if (Cout1 != 1) {
      Serial.print(" Channel 1  : ");
      Serial.println("Alarm UPS Data Center Room,[General Alarm],Please Contact K.ChocSiRi 081-9131422");
      String message = "Alarm UPS Data Center Room,[General Alarm],Please Contact K.ChocSiRi 081-9131422";  //เกิดความผิดปกติของ UPS DATA Center Room [General Alarm],โปรดติดต่อ คุณ โชคสิริ 081-9131422
      Line_Notify(message);
      Cout1 = 1;
    }
  }
  if (buttonState1 == LOW) {
    if (Cout1 != 0) {
      Serial.print(" Channel 1  : ");
      Serial.println("UPS Data Center Room is Normal");
      String message = "UPS Data Center Room is Normal";
      Line_Notify(message);
      Cout1 = 0;
    }
  }
  // สิ้นสุด ----------------------- ช่องที่ 1 D2 ---------------------------
  // เริ่มต้น ----------------------- ช่องที่ 2 D3 ---------------------------
  if (buttonState2 == LOW) {
    if (Cout2 != 1) {
      Serial.print(" Channel 2  : ");
      Serial.println("Alarm UPS Data Center Room,[Load On Main],Please Contact K.ChocSiRi 081-9131422");
      String message = "Alarm UPS Data Center Room,[Load On Main],Please Contact K.ChocSiRi 081-9131422"; //เกิดความผิดปกติของ UPS DATA Center Room [Load On Main],โปรดติดต่อ คุณ โชคสิริ 081-9131422
      Line_Notify(message);
      Cout2 = 1;
    }
  }
  if (buttonState2 == HIGH) {
    if (Cout2 != 0) {
      Serial.print(" Channel 2  : ");
      Serial.println("UPS Data Center Room is Normal");
      String message = "UPS Data Center Room is Normal"; // UPS DATA Center Room กลับสู่ภาวะปกติเรียบร้อยแล้ว.
      Line_Notify(message);
      Cout2 = 0;
    }
  }
  // สิ้นสุด ----------------------- ช่องที่ 2 D3 ---------------------------

}
void Line_Notify(String message) {
  WiFiClientSecure client;

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return;
  }
  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Connection: close\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message;
  // Serial.println(req);
  client.print(req);
  delay(20);
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
}

