#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
int Rx_Pin = D2;  // กำหนดขา D2 เป็นขารับข้อมูล
int Tx_Pin = D3;  // กำหนดขา D3 เป็นขารส่งข้อมูล
SoftwareSerial ArduinoSerial(Rx_Pin, Tx_Pin);  // กำหนดขารับส่งข้อมูล RX, TX
String dataIn;  // ตัวแปรสำหรับเก็บข้อมูลบที่ได้รับ

void splint_string(char sz[]) { // สร้างฟังชันต์ชื่อ splint_string กำหนดตัวแปรนำเข้าชื่อ sz ชนิด char แบบอาเรย์

  char *p = sz;  // สร้างตัวแปรชื่อ p ชนิด Pointer มีค่าเท่ากับ sz
  char *str;  // สร้างตัวแปรชื่อ str ชนิด Pointer
  int counter = 0;  // สร้างตัวแปรชื่อ counter ชนิด int สำหรับทำการนับครั้งที่ตัด

  while ((str = strtok_r(p, "/", &p)) != NULL) { // วนทำลูป while ซ้ำ โดยเรียกฟังชันต์ strtok_r() โดยทำการตัดค่าใน p เมื่อเจอเครื่องหมาย','
    counter++;
    if (counter == 1) {
      Serial.print(String("Humidity:") + String(" = "));  // แสดงผลจำนวนครั้งที่ตัด
      Serial.println(str); // แสดงผลค่าที่ตัดได้
      root["humidity"] = str; //ความชื้น
    } else if (counter == 2) {
      Serial.print(String("Temperature:") + String(" = "));  // แสดงผลจำนวนครั้งที่ตัด
      Serial.println(str); // แสดงผลค่าที่ตัดได้
      root["temperature"] = str; //อุณหภูมิ
    } else if (counter == 3) {
      Serial.print(String("Pressure:") + String(" = "));  // แสดงผลจำนวนครั้งที่ตัด
      Serial.println(str); // แสดงผลค่าที่ตัดได้
      root["pressure"] = str; //อุณหภูมิ
    }
  }
  counter = 0;  // เคลียร์ค่าใน counter เป็น 0
}

void setup() {
  WiFi.mode(WIFI_STA);// connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  pinMode(Rx_Pin, INPUT);  // กำหนดขา Rx_Pin เป็น INPUT
  pinMode(Tx_Pin, OUTPUT);  // กำหนดขา Tx_Pin เป็น OUTPUT
  Serial.begin(9600);  // กำหนด baud rate ของ Serial เป็น 9600
  ArduinoSerial.begin(115200);  // กำหนด baud rate ของ ArduinoSerial เป็น 115200
}

void loop() {
  ArduinoSerial.print("Hi Arduino");  // ส่งค่า "Hi Arduino" ออกไปยัง ArduinoSerial
  ArduinoSerial.print("\n");  // ส่งค่า "\n" ออกไปยัง ArduinoSerial เพื่อขึ้นบรรทัดใหม่
  delay(1000);  // หน่วงเวลา 100 Millisecond

  while (ArduinoSerial.available() > 0) { // วนลูปหากมีค่าส่งเข้ามาใน ArduinoSerial
    char recieved = ArduinoSerial.read();  // สร้างตัวแปรชื่อ recieved เป็นชนิด char เพื่อรับข้อมูลจาก ArduinoSerial
    dataIn += recieved;  // นำค่าที่ได้จาก recieved มารวมใว้ใวตัวแปร dataIn
    if (recieved == '\n') {  // หากค่าที่รับได้เป็น '\n'
      char charBuf[100];
      dataIn.toCharArray(charBuf, 100);  // คัดลอกอักขระของชุดอักขระไปยังตัวแปร charBuf
      splint_string(charBuf); // เรียกใช้งานฟังชั่น Splint String
      dataIn = "";  // กำหนดให้ dataIn ว่าง
    }
  }
}
