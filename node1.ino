//WIFI
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define WIFI_SSID  "6021607"
#define WIFI_PASS  "17401449"

char auth[] = "a5e4e286dfc748519add1334ecf149ee"; // NODE1

//Magnetic Door switch
const int buzzer = 16;
const int DoorGpioPin = 0; // D3 of Node MCU
const int ledGpioPinRed = 2;  // D4 of Node MCU
const int ledGpioPinBlue = 4;  // D2 of Node MCU
int ledState = 0;    //LEDstate
int doorState = 0;   //Doorstate
#define closed 0
#define opened 1


//RFID PN532
#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
PN532_SPI pn532spi(SPI, 5); // node ss ขา D1 GPIO = 5 arduino ขา 10
PN532 nfc(pn532spi);
uint8_t currentblock = 1; // block write select
uint8_t data[20];         // block write data

//SERVO
#include <Servo.h>
Servo myservo;
int servo = 15;
int buttonStatelock;

//LINE
void Line_Notify(String message) ;
#define LINE_TOKEN "lTdXXOfUCH00vgQWgjRNdt83bVptSk0yQysFezcVIvp"

int door = 0;                         // Start 0
int Servos = 0;                       // Start 0
int keycard = 2;                      // Start 2
int app = 2;

void setup() {

  Serial.begin(9600);
  // WIFI
  Blynk.begin(auth, WIFI_SSID, WIFI_PASS, "blynk-cloud.com", 8442);

  //Magnetic Door switch
  pinMode(DoorGpioPin, INPUT);
  pinMode(ledGpioPinRed, OUTPUT);
  pinMode(ledGpioPinBlue, OUTPUT);

  //SERVO
  myservo.attach(servo);
  myservo.write(0);

  //RFID PN532
  nfc.begin();
  Serial.println("Readdata!");
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // HALT
  }
  nfc.SAMConfig();
  Serial.println("Waiting for Card ...");
}

BLYNK_WRITE(V1) {
  buttonStatelock = param.asInt();
  if (buttonStatelock == 0) {
    app = 0;
    sclose();
  }
  else if (buttonStatelock == 1) {
    app = 1;
    sopen();
  }
}

void loop()
{
  //Blynk
  Blynk.run();
  Blynk.virtualWrite(10, buttonStatelock);
  Blynk.virtualWrite(11, door);

  //Call function RFID
  RFID();

  //Magnetic Door switch
  if ( digitalRead(DoorGpioPin) == HIGH && doorState == opened) { // Print button pressed message.
    //SendDoorCloseNotification();
    Serial.println("Close");
    doorState = closed;
    door = 0;     // ประตูปิด
    String message = "Chanal 1 : DoorClose";
    Line_Notify(message);

    buzzeroff(); //off buzzer
    ledBlueon(); //blue on
    ledRedoff(); //red off
  }
  if ( digitalRead(DoorGpioPin) == LOW && doorState == closed) {
    //SendDoorOpenNotification();
    Serial.println("Open");
    doorState = opened;
    door = 1;     // ประตูเปิด
    String message = "Chanal 1 : DoorOpen";
    Line_Notify(message);

    buzzeron();  //on buzzer
    ledRedon(); //Red on
    ledBlueoff(); //blue off
  }

  //งัดประตูปกติ
  //ถ้าประตูเปิดโดยที่ ประตูล๊อกยังล๊อก ให้แจ้งเตือน
  if (door == 1 && Servos == 0) {
    Serial.println("@@Notify Door !!");
    Serial.print("Door :"); Serial.println(door);
    Serial.print("Servo :"); Serial.println(Servos);
    buzzeron();
    sclose();

  }
  //ถ้าประตูปิดโดยที่ ประตูล๊อกยังล๊อก ไม่ต้องแจ้งเตือน
  if (door == 0 && Servos == 0) {
    Serial.println("@@Normal on LOCK**");
    Serial.print("Door :"); Serial.println(door);
    Serial.print("Servo :"); Serial.println(Servos);
    buzzeroff();
    sclose();
  }

  //เอาคีย์ไม่ทราบมาแตะ
  //เอาคีย์การ์ดมาไม่ทราบมาแตะ ประตูล๊อกอยู่ ให้แจ้งเตือน
  if (keycard == 0 && Servos == 0) {
    Serial.println("Notify Keycard + Servo CASE :1");
    Serial.print("Keycard :"); Serial.println(keycard);
    Serial.print("Servo :"); Serial.println(Servos);
    buzzeron();                   //เปิดเสียง
    sclose();                     //ล๊อกประตูไว้อย่างนั่น
  }
  //เอาคีย์ไม่ทราบมาแตะ ประตูปิดอยู่ ประตูล๊อกอยู่ ให้แจ้งเตือน
  if (keycard == 0 && door == 0 && Servos == 0) {
    Serial.println("Notify Keycard + Door + Servo CASE :2");
    Serial.print("Keycard :"); Serial.println(keycard);
    Serial.print("Servo :"); Serial.println(Servos);
    buzzeron();                   //เปิดเสียง
    sclose();                     //ล๊อกประตูไว้อย่างนั่น
  }
  //เอาคีย์ไม่ทราบมาแตะ งัดประตู ประตูล๊อกอยู่ ให้แจ้งเตือน
  if (keycard == 0 && door == 1 && Servos == 0) {
    Serial.println("Notify Keycard + Door + Servo CASE :6");
    Serial.print("Keycard :"); Serial.println(keycard);
    Serial.print("Servo :"); Serial.println(Servos);
    buzzeron();                  //เปิดเสียง
    sclose();                    //ล๊อกประตูไว้อย่างนั่น
  }

  // เปิดปกติ
  //เอาคีย์การ์ดมาแตะ ประตูล๊อก ไม่ต้องแจ้งเตือน และปลดล๊อกประตู
  if (keycard == 1 && Servos == 0) {
    Serial.println("@@Normal Open :C0");
    Serial.print("Keycard :"); Serial.println(keycard);
    Serial.print("Servo :"); Serial.println(Servos);
    buzzeroff();
    sopen();      // เปิดประตู set servo = 1
    delay(7000);
    sclose();     // ปิดประตู set servo = 0
    keycard = 2;  // คีย์การ์ดกลับไปเป็น 2 เพราะประตูล๊อกแล้ว
  }
  //เอาคีย์การ์ดมาแตะ ประตูล๊อก ประตูปิด ไม่ต้องแจ้งเตือน และปลดล๊อกประตู
  if (keycard == 1 && Servos == 0 && door == 0) {
    Serial.println("@@Normal Open :C1");
    Serial.print("Keycard :"); Serial.println(keycard);
    Serial.print("Servo :"); Serial.println(Servos);
    buzzeroff();
    sopen();      // เปิดประตู set servo = 1
    delay(7000);
    sclose();     // ปิดประตู set servo = 0
    keycard = 2;  // คีย์การ์ดกลับไปเป็น 0 เพราะประตูล๊อกแล้ว
  }

  //ปลดล๊อก ประตูล๊อก ไม่ต้องแจ้งเตือน และปลดล๊อกประตู
  if (app == 1 && Servos == 0) {
    Serial.println("@@Normal Open :C2");
    Serial.print("app :"); Serial.println(app);
    Serial.print("Servo :"); Serial.println(Servos);
    buzzeroff();
    app = 2;  // คีย์การ์ดกลับไปเป็น 0 เพราะประตูล๊อกแล้ว
  }
  //เอาปลดล๊อก ประตูล๊อก ประตูปิด ไม่ต้องแจ้งเตือน และปลดล๊อกประตู
  if (app == 1 && Servos == 1 && door == 1) {
    Serial.println("@@Normal Open :C3");
    Serial.print("app :"); Serial.println(app);
    Serial.print("Servo :"); Serial.println(Servos);
    buzzeroff();
    app = 2;  // คีย์การ์ดกลับไปเป็น 0 เพราะประตูล๊อกแล้ว
  }

}
void RFID()
{
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    //nfc.PrintHex(uid, uidLength);
    Serial.print("Read Card");
  }
  if (uidLength == 4) {
    uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // key
    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, currentblock, 1, keya); // select block
    if (success) {
      success = nfc.mifareclassic_ReadDataBlock(currentblock, data); // Readblock
      if (success) {
        Serial.println();
        Serial.print("DATA :"); Serial.write(data, HEX);
        Serial.println();
        char check[20];
        int num;
        String Text;
        for (int i = 0 ; i < sizeof(data); i++ ) {
          String text = String(data[i], DEC);
          num = text.toInt();
          check[i] = num;
          Text += check[i];
        }
        Serial.print("Text :");
        Serial.println(Text);
        if (Text == "ADMIN") {
          Serial.println("OK : ADMIN!!");
          keycard = 1;
          Serial.println(keycard);
          String message = "Chanal 1 Login: ADMIN";
          Line_Notify(message);

        }
        else if (Text == "") {
          keycard = 0;
          Serial.println(keycard);
          Serial.println("NOT : Unknow!!");
          String message = "Chanal 1 Try to Login : Unknow";
          Line_Notify(message);

        } else {
          keycard = 0;
          Serial.println(keycard);
          Serial.println("NOT : Unknow!!");
          String message = "Chanal 1 Try to Login : Unknow";
          Line_Notify(message);

        }
        Serial.println();
      } else {
        Serial.println("Ooops ..");
      }
    }
  } else {
    Serial.println("**Start Wait key Card **");
  }
  delay(500);
}
void sopen() {
  myservo.write(40);
  Servos = 1;
}
void sclose() {
  myservo.write(100);
  Servos = 0;
}
void ledRedon()
{
  digitalWrite(ledGpioPinRed, HIGH);
}
void ledRedoff()
{
  digitalWrite(ledGpioPinRed, LOW);
}
void ledBlueon()
{
  digitalWrite(ledGpioPinBlue, HIGH);
}
void ledBlueoff()
{
  digitalWrite(ledGpioPinBlue, LOW);
}
void buzzeron()
{
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(200);
}
void buzzeroff()
{
  digitalWrite(buzzer, LOW);
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
  // Serial.println("-------------");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    //Serial.println(line);
  }
  Serial.print("\n------ Send Successfully ------");
  Serial.println();
}

