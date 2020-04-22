#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include <SD.h>
#include <time.h>

SoftwareSerial NodeSerial(D2, D3); // RX | TX
ESP8266WebServer server(8089);

//const char* ssid = "simlee3";
//const char* password = "06083105";


const char* ssid = "Newdawn-1";
const char* password = "Newdawn999";

int timezone = 7 * 3600; //ตั้งค่า TimeZone ตามเวลาประเทศไทย
int dst = 0; //กำหนดค่า Date Swing Time
String data;

int i = 0;

//บันทึกข้อมูลลงsd card
File myFile; // สร้างออฟเจก File สำหรับจัดการข้อมูล
const int chipSelect = D8;
int temp;
int temp2;
String x;

//ตัวแปรรับค่าที่ส่งมาจากบอร์ด
int val1, val2;
float S_data1, S_data2;

//ตัวแปรส่งเข้าเว็บ
String Ands = "&";
String ID = "ID=";
String ID_No = "1150";
String KeyESP = "KeyESP=";
String NameESP = "Computer Newdawn";
String valueSensor = "Value=";
String value = "";
String ReadSd = "";

String bow = "Hello";

void setup() {
  // put your setup code here, to run once:
  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  Serial.begin(9600);
  NodeSerial.begin(57600);

  //เช็ค wifi
  WiFi.begin(ssid, password);  //Connect to the WiFi network

  //เช็ด SD
  while (!Serial) {
    ; // รอจนกระทั่งเชื่อมต่อกับ Serial port แล้ว สำหรับ Arduino Leonardo เท่านั้น
  }
  Serial.println("Initializing SD card...");
  pinMode(SS, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }

  //ดึงเวลาจาก Server
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  while (!time(nullptr)) {
    Serial.print("*");
    delay(1000);
  }
}

void loop() {
  temp = getValue();
  Serial.println("Frist :" + String(temp));
  WriteSD(temp);
  time_t now = time(nullptr);
  data = ctime(&now);
  delay(1000);
  if (WiFi.status() != WL_CONNECTED) {  //Wait for connection
    delay(500);
    Serial.print(++i); Serial.print(' ');
    // Serial.println(data);
  }
  else {
    ReadSD();
    myPOST(x);
    //ReadSD();
    Serial.println("my post : " + value );
    RemoveSD();
    Serial.println(data);
  }
  //  Serial.println("Connection established!");
  //  Serial.print("IP address: ");
  //  Serial.println(WiFi.localIP());  //Print the local IP
  Serial.println("Last : " + value);
  //RemoveSD();

  delay(2000);
}

int getValue() {
  while (NodeSerial.available() > 0)
  {
    val1 = NodeSerial.parseInt();
    S_data1 = NodeSerial.parseFloat();
    if (NodeSerial.read() == '\n')
    {
      Serial.print("NodeMCU or ESP8266"); Serial.print(" : ");
      Serial.print(val1); Serial.print(" : ");
      Serial.println(S_data1);
    }
    delay(50);
  }
  delay(10);
  //val = 5555;
  return val1;
}

void WriteSD(int txt) {
  myFile = SD.open("test1.txt", FILE_WRITE); // เปิดไฟล์ที่ชื่อ test.txt เพื่อเขียนข้อมูล โหมด FILE_WRITE
  if (myFile) {
    myFile.print(data);
    myFile.print("\t");
    myFile.println(txt); // สั่งให้เขียนข้อมูล
    myFile.close(); // ปิดไฟล์
  } else {
    // ถ้าเปิดไฟลืไม่สำเร็จ ให้แสดง error
    Serial.println("Not WriteSD");
  }
  // myFile.close(); // ปิดไฟล์
}

void ReadSD() {
  // เปิดไฟล์เพื่ออ่าน
  myFile = SD.open("test1.txt"); // สั่งให้เปิดไฟล์ชื่อ test.txt เพื่ออ่านข้อมูล
  if (myFile) {
    // อ่านข้อมูลทั้งหมดออกมา
    while (myFile.available() > 0) {
      //Serial.write(myFile.read());
      Serial.println("Read Success");
      x = myFile.readStringUntil('\n');
      //x.toInt();
      //x = myFile.parseInt();
      Serial.println(x);
    }
    myFile.close(); // เมื่ออ่านเสร็จ ปิดไฟล์
  } else {
    // ถ้าอ่านไม่สำเร็จ ให้แสดง error
    //Serial.println("Not ReadSD");
  }
  delay(1000);
}

void RemoveSD() {
  if (SD.exists("test1.txt"))
  {
    SD.remove("test1.txt");
    Serial.println("file removed");
  }
  else
  {
    Serial.println("no file to remove");
  }
}

void myPOST(String data) {
  value = data;
  HTTPClient http;
  http.begin("http://192.168.0.63/testpassvalueonweb.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.POST(valueSensor + value);
  http.writeToStream(&Serial);
  Serial.println();
  http.end();

}
