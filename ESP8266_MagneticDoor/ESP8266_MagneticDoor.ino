#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>
#define SSID "--------------------"
#define PASSWORD "--------------------"
#define LINE_TOKEN "--------------------"
#define DoorGpioPin D1
#define ledGpioPin D2
#define closed 0
#define opened 1

int doorState = 0;
int ledState = 0;

void setup(){
  pinMode(DoorGpioPin, INPUT);
  pinMode(ledGpioPin, OUTPUT);
  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.begin(115200);
  Serial.println();
  Serial.println(LINE.getVersion());

  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n", SSID);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());

  // กำหนด Line Token
  LINE.setToken(LINE_TOKEN);

  // ตัวอย่างส่งข้อความ
  LINE.notify("เชื่อมต่อแล้ว");
}
void loop(){
  if (digitalRead(DoorGpioPin) == HIGH && doorState == opened)
  { // Print button pressed message.
    LINE.notify("ปิด");
    Serial.println("Close");
    doorState = closed;
    ledState = 1; //on
    digitalWrite(ledGpioPin, ledState);
  }
  if (digitalRead(DoorGpioPin) == LOW && doorState == closed)
  {
    LINE.notify("เปิด");
    Serial.println("Open");
    doorState = opened;
    ledState = 0; //off
    digitalWrite(ledGpioPin, ledState);
  }
}
