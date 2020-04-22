#include <SoftwareSerial.h>
DHT dht(DHTPIN, DHTTYPE);

int Rx_Pin = 3;  // กำหนดขา 11 เป็นขารับข้อมูล
int Tx_Pin = 2;  // กำหนดขา 10 เป็นขารส่งข้อมูล
SoftwareSerial NodeSerial(Rx_Pin, Tx_Pin);  // กำหนดขารับส่งข้อมูล RX, TX

String dataIn;  // ตัวแปรสำหรับเก็บข้อมูลบที่ได้รับ

void setup() {
  pinMode(Rx_Pin, INPUT);  // กำหนดขา Rx_Pin เป็น INPUT
  pinMode(Tx_Pin, OUTPUT);  // กำหนดขา Tx_Pin เป็น OUTPUT
  Serial.begin(9600);  // กำหนด baud rate ของ Serial เป็น 9600
  NodeSerial.begin(115200);  // กำหนด baud rate ของ NodeSerial เป็น 115200
}

void loop() {
  char str[100];  //สร้างตัวแปร str ชนิด char แบบอาเรย์สำหรับเก็บค่าชุดตัวอักษรที่จะส่ง
  // จัดเรียงข้อมูลจาก Sensor เป็นชุดเดียวโดยใช้ใช้คำสั่ง sprintf
  sprintf(str, "%s/%s/%s", String(dht.readHumidity()).c_str(), String(dht.readTemperature()).c_str(), String(pa).c_str());
  Serial.println(str);  // แสดงค่าที่จัดเก็บออกไปยัง Serial Monitor
  NodeSerial.print(str);  // ส่งค่าใน str ออกไปยัง ArduinoSerial
  NodeSerial.print("\n");  // ส่งค่า "\n" ออกไปยัง ArduinoSerial เพื่อขึ้นบรรทัดใหม่
  delay(1000);  // หน่วงเวลา 1000 Millisecond
}
