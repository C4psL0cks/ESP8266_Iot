#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
ESP8266WiFiMulti WiFiMulti;
const char *ssid = "SSID";
const char *password = "PASSWORD";
// ************************************************************************** ควบคุม WI-FI //
#include "Adafruit_GFX.h"    // Core graphics library
#include "Adafruit_ST7735.h" // Hardware-specific library
#define TFT_CS D1            //ขา I/O เชื่อกับจอ TFT กับ NodeMCU D1
#define TFT_RST (-1)         //ขา I/O เชื่อกับจอ TFT กับ NodeMCU RST
#define TFT_DC D0            //ขา I/O เชื่อกับจอ TFT กับ NodeMCU D0
#define TFT_SCLK D5          //ขา I/O เชื่อกับจอ TFT กับ NodeMCU D5
#define TFT_MOSI D7          //ขา I/O เชื่อกับจอ TFT กับ NodeMCU D7
#define ROTATION_OPTION 1    // 0=แนวตั้ง,1=แนวนอน,2=แนวตั้งกลับหัว, 3=แนวนอนกลับหัว
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
int w, h;
// ************************************************************************** ควบคุมจอแสดงผล TFT //
#include <Adafruit_MAX31856.h>
Adafruit_MAX31856 max = Adafruit_MAX31856(D2, D3, D4, D6); //ขา I/O NodeMCU ที่ใช้อ่านค่าอุณหภูมิ
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31856 max = Adafruit_MAX31856(10);
// ************************************************************************** ควบคุมการอ่านค่าอุณหภูมิ //
void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // ************************************************************************** ควบคุม WI-FI //
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(ROTATION_OPTION);
  w = tft.width();
  h = tft.height();

  tft.fillScreen(ST7735_BLACK);
  tft.drawRect(0, 0, w, h, ST7735_GREEN);
  // ************************************************************************** ควบคุมการแสดงผล TFT //
  Serial.println("MAX31856 thermocouple test");
  max.begin();
  max.setThermocoupleType(MAX31856_TCTYPE_R); //เลือก Type Thermocouple ที่ใช่วัด เช่น R,B,E,J,K,N,S,T เป็นต้น
  Serial.print("Thermocouple type: ");
  switch (max.getThermocoupleType())
  {
  case MAX31856_TCTYPE_B:
    Serial.println("B Type");
    break;
  case MAX31856_TCTYPE_E:
    Serial.println("E Type");
    break;
  case MAX31856_TCTYPE_J:
    Serial.println("J Type");
    break;
  case MAX31856_TCTYPE_K:
    Serial.println("K Type");
    break;
  case MAX31856_TCTYPE_N:
    Serial.println("N Type");
    break;
  case MAX31856_TCTYPE_R:
    Serial.println("R Type");
    break;
  case MAX31856_TCTYPE_S:
    Serial.println("S Type");
    break;
  case MAX31856_TCTYPE_T:
    Serial.println("T Type");
    break;
  case MAX31856_VMODE_G8:
    Serial.println("Voltage x8 Gain mode");
    break;
  case MAX31856_VMODE_G32:
    Serial.println("Voltage x8 Gain mode");
    break;
  default:
    Serial.println("Unknown");
    break;
  }
  // ************************************************************************** ควบคุมการอ่านค่าอุณหภูมิ //
}

void loop()
{
  ArduinoOTA.handle(); //อัพโหลดโปรแกรมระบบไร้สายผ่าน Wi-Fi ( OTA )
  int Temp = (max.readThermocoupleTemperature());
  float Temp1 = (max.readThermocoupleTemperature());
  uint8_t fault = max.readFault();
  if (fault)
  {
    if (fault & MAX31856_FAULT_CJRANGE)
      Serial.println("Cold Junction Range Fault");
    if (fault & MAX31856_FAULT_TCRANGE)
      Serial.println("Thermocouple Range Fault");
    if (fault & MAX31856_FAULT_CJHIGH)
      Serial.println("Cold Junction High Fault");
    if (fault & MAX31856_FAULT_CJLOW)
      Serial.println("Cold Junction Low Fault");
    if (fault & MAX31856_FAULT_TCHIGH)
      Serial.println("Thermocouple High Fault");
    if (fault & MAX31856_FAULT_TCLOW)
      Serial.println("Thermocouple Low Fault");
    if (fault & MAX31856_FAULT_OVUV)
      Serial.println("Over/Under Voltage Fault");
    if (fault & MAX31856_FAULT_OPEN)
      Serial.println("Thermocouple Open Fault");
  }
  Serial.print("Thermocouple Temp: ");
  Serial.println(Temp1);

  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(2);
  tft.setCursor(25, 10);
  tft.println("TM 1 TEMP");
  tft.setTextSize(2);
  tft.println(" ");
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(5);
  tft.setCursor(20, 50);
  tft.println(Temp);
  tft.setTextSize(2);
  tft.println(" ");
  tft.setTextSize(1);
  tft.setTextColor(ST7735_YELLOW);
  tft.println(" HLZ COMPUTER ENGINEER");

  if ((WiFiMulti.run() == WL_CONNECTED))
  {
    HTTPClient http;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    String url = "http://192.168.0.0/burning/get/index1.php?data=" + String("Ax") + Temp1 + String("x");
    // IP = server (ฐานข้อมูล) ที่จะใช้ส่งข้อมูลค่าอุณหภูมิไปเก็บ / ไฟล์ PHP / ส่งแบบ get / index.php?data= Ax =ขั้นคอลัม / ค่าอุณหภูมิที่อ่านได้ / X = เปลี่ยนบรรทัด
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Serial.println(url);
    http.begin(url); //HTTP
    int httpCode = http.GET();
    if (httpCode = Temp1)
    {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK)
      {
        String payload = http.getString();
        Serial.println(payload);
      }
    }
    else
    {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  delay(10000);
}
