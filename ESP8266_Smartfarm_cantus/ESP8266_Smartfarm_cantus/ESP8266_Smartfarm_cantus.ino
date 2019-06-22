#include <ESP8266WiFi.h>
#include <time.h>
#include <MCP3008.h>
#include <OneWire.h>
#include <TridentTD_LineNotify.h>
#include <Servo.h>

#define CS_PIN D8
#define CLOCK_PIN D5
#define MOSI_PIN D7
#define MISO_PIN D6
#define WIFI_SSID "6021607"
#define WIFI_PASS  "17401449"
#define LINE_TOKEN  "vMIl5g0XKEPgMZdQCPeff8DKiYLE7jgrRpfRptZikrx"
#define relaypinIN1_Water D0
#define relaypinIN2_Fan D1
#define Sensor_Pin_Moisture 0
#define Sensor_Pin_LDR 1
#define Sensor_Pin_Temperature D2
#define Sensor_Pin_Servo D3

byte i;
byte present = 0;
byte type_s;
byte data[12];
byte addr[8];

int ValueLDR = 0;
int sensorValue;

char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";

float volts;
float celsius;
float Value_Temperature = 0;
float Value_Moisture_percentage = 0;

double Fuzzy_Average_Water;
double Fuzzy_Average_Fan;
double Fuzzy_Average_Windows;
double ResultTempFuzzy = 0;
double ResultLDRFuzzy = 0;
double ResultSoilFuzzy = 0;
double ResultAverage = 0;
double Fuzzy_Average;

bool checkwater1 = true;
bool checkwater2 = true;
bool checkwater3 = true;
bool checkwater4 = true;
bool checkfan1 = true;
bool checkfan2 = true;
bool checkwindows1 = true;
bool checkwindows2 = true;
bool checkwindows3 = true;

ADC_MODE(ADC_VCC);
OneWire ds(Sensor_Pin_Temperature);
MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);
Servo myservo;

void setup() {

  Serial.begin(115200);
  myservo.attach(Sensor_Pin_Servo);
  myservo.write(0);
  pinMode(relaypinIN1_Water, OUTPUT);
  pinMode(relaypinIN2_Fan, OUTPUT);
  WaterOff();
  FanOff();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
  Serial.println("");

  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  LINE.setToken(LINE_TOKEN);
}

void loop() {

  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  Serial.println("************************************************");
  Serial.print("DATE : " + String(ctime(&now)));

  sensorValue = ESP.getVcc();
  volts = (float)sensorValue / 1023.00;
  Serial.println("Volts in ESP8266 : " + String(volts));

  Value_Moisture_percentage = ( 100.00 - ( (adc.readADC(Sensor_Pin_Moisture) / 1023.00) * 100.00 ) );
  Serial.println("Value_Moisture_percentage : " + String(Value_Moisture_percentage));

  ValueLDR = adc.readADC(Sensor_Pin_LDR);
  Serial.println("ValueLDR : " + String(ValueLDR));

  Value_Temperature = temperature();
  Serial.println("Value_Temperature : " + String(Value_Temperature));

  Fuzzy_Average_Water = Water(Value_Temperature, ValueLDR, Value_Moisture_percentage); //Average water
  Serial.print("Fuzzy_Average_Water : " + String(Fuzzy_Average_Water));

  Fuzzy_Average_Fan = Fan(Value_Temperature, Value_Moisture_percentage);               //Average Fan
  Serial.print(" Fuzzy_Average_Fan : " + String(Fuzzy_Average_Fan));

  Fuzzy_Average_Windows = Windows(ValueLDR, Value_Moisture_percentage);                //Average windonws
  Serial.print(" Fuzzy_Average_Windows : " + String(Fuzzy_Average_Windows));

  Serial.println();

  Serial.println("************************************************");


  /// Water
  if (p_tm->tm_hour >= 0 && p_tm->tm_hour <= 8) { // 00.00 - 08.00
    Serial.println("---------------------------");
    Serial.println("Water :00.00 - 08.00 ");
    if (Fuzzy_Average_Water > 0.25 && Fuzzy_Average_Water < 0.35) {
      WaterOn();
      Serial.println("-->WATERON");
      if (checkwater1) {
        //LINE.notify("เปิดน้ำ");
        //LINE.notify(p_tm->tm_hour);
        //LINE.notify(Fuzzy_Average_Water, 5);
        checkwater1 = false;
      }
      Serial.println("---------------------------");
    }
    else {
      WaterOff();
      Serial.println("-->WATEROFF");
      if (checkwater2) {
        //LINE.notify("ปิดน้ำ");
        //LINE.notify(p_tm->tm_hour);
        //LINE.notify(Fuzzy_Average_Water, 5);
        checkwater2 = false;
      }
      Serial.println("---------------------------");
    }
  }

  if (p_tm->tm_hour >= 18 && p_tm->tm_hour <= 23) { // 18.00 - 23.00
    Serial.println("---------------------------");
    Serial.println("Water : 18.00 - 23.00 ");
    if (Fuzzy_Average_Water >= 1.00) {
      WaterOn();
      Serial.println("-->WATERON");
      if (checkwater3) {
        //LINE.notify("เปิดน้ำ");
        //LINE.notify(p_tm->tm_hour);
        //LINE.notify(Fuzzy_Average_Water, 5);
        checkwater3 = false;
      }
      Serial.println("---------------------------");
    }
    else {
      WaterOff();
      Serial.println("-->WATEROFF");
      if (checkwater4) {
        //LINE.notify("ปิดน้ำ");
        //LINE.notify(p_tm->tm_hour);
        //LINE.notify(Fuzzy_Average_Water, 5);
        checkwater4 = false;
      }
      Serial.println("---------------------------");
    }
  }

  /// Fan
  if (p_tm->tm_hour >= 12 && p_tm->tm_hour <= 23) { // 12.00 - 23.00
    Serial.println("---------------------------");
    Serial.println("Fan : 12.00 - 23.00 ");
    if (Fuzzy_Average_Fan >= 0.50) {
      FanOn();
      Serial.println("-->FANON");
      if (checkfan1) {
        //LINE.notify("เปิดพัดลม");
        //LINE.notify(p_tm->tm_hour);
        //LINE.notify(Fuzzy_Average_Fan, 5);
        checkfan1 = false;
      }
      Serial.println("---------------------------");
    }
    else {
      FanOff();
      Serial.println("-->FANOFF");
      if (checkfan2) {
        //LINE.notify("ปิดพัดลม");
        //LINE.notify(p_tm->tm_hour);
        //LINE.notify(Fuzzy_Average_Fan, 5);
        checkfan2 = false;
      }
      Serial.println("---------------------------");
    }
  }

  /// Windows
  if (p_tm->tm_hour >= 0 && p_tm->tm_hour < 14) { // 00.00 - 14.00
    Serial.println("---------------------------");
    Serial.println("Windows: 00.00 - 13.00 ");
    if (Fuzzy_Average_Windows > 0.70 && Fuzzy_Average_Windows < 1.50) {
      WindowsOn();
      Serial.println("-->WINDOWOpen");
      if (checkwindows1) {
        //LINE.notify("เปิดม่าน");
        //LINE.notify(p_tm->tm_hour);
        //LINE.notify(Fuzzy_Average_Windows, 5);
        checkwindows1 = false;
      }
      Serial.println("---------------------------");
    }
    else {
      WindowsOff();
      Serial.println("-->WINDOWClose");
      if (checkwindows2) {
        //LINE.notify("ปิดม่าน");
        //LINE.notify(p_tm->tm_hour);
        //LINE.notify(Fuzzy_Average_Windows, 5);
        checkwindows2 = false;
      }
      Serial.println("---------------------------");
    }
  }
  if (p_tm->tm_hour >= 14) { //หลังไปแล้วให้ปิด 14.00
    Serial.println("---------------------------");
    Serial.println("Windows: 14.00 ");
    if (Fuzzy_Average_Windows > 0.70 && Fuzzy_Average_Windows < 1.50) {
      WindowsOff();
      Serial.println("-->WINDOWClose");
      if (checkwindows3) {
        //LINE.notify("ปิดม่าน");
        //LINE.notify(p_tm->tm_hour);
        //LINE.notify(Fuzzy_Average_Windows, 5);
        checkwindows3 = false;
      }
      Serial.println("---------------------------");
    }
  }

}
void WaterOn() {
  digitalWrite(relaypinIN1_Water, LOW);
}
void WaterOff() {
  digitalWrite(relaypinIN1_Water, HIGH);
}
void FanOn() {
  digitalWrite(relaypinIN2_Fan, LOW);
}
void FanOff() {
  digitalWrite(relaypinIN2_Fan, HIGH);
}
void WindowsOn() {
  myservo.write(70);
}
void WindowsOff() {
  myservo.write(0);
}

float One(int input, int zmin, int zmax) {
  float result = (float(input - zmin) / float(zmax - zmin));
  return result;
}
float Zero(int input, int zmin, int zmax) {
  float result = float(1 - (float(input - zmin) / float(zmax - zmin)));
  return result;
}
float average_water(float ResultTempFuzzy, float ResultLDRFuzzy, float ResultSoilFuzzy) {
  float avg_water = (ResultTempFuzzy + ResultLDRFuzzy + ResultSoilFuzzy) / 3;
  return avg_water;
}
float average_fan(float ResultTempFuzzy, float ResultSoilFuzzy) {
  float avg_fan = (ResultTempFuzzy + ResultSoilFuzzy) / 2;
  return avg_fan;
}
float average_windows(float ResultLDRFuzzy, float ResultSoilFuzzy) {
  float avg_windows = (ResultLDRFuzzy + ResultSoilFuzzy) / 2;
  return avg_windows;
}

double Water(float Value_Temperature, int ValueLDR, float Value_Moisture_percentage) {

  float resultTemp = 0;
  float resultLDR = 0;
  float resultSoil = 0;
  float Averagewater_ALL;

  if (Value_Temperature > 27) {
    resultTemp = One(Value_Temperature, 27, 40);  // ร้อนถึงร้อนมาก = 1 คือเปิด
    //Serial.println("resultTemp :" + String(resultTemp));
    //0.31
    if (resultTemp > 0.30) {
      resultTemp = 1;
    }
  }
  else {
    resultTemp = Zero(Value_Temperature, 0, 27);  // ร้อนน้อย = 0 คือไม่เปิด
    //Serial.println("resultTemp :" + String(resultTemp));
    //0.19
    if (resultTemp < 0.20) {
      resultTemp = 0;
    }
  }

  //Serial.println("resultTemp :" + String(resultTemp));

  if (ValueLDR >= 200 && ValueLDR <= 700) {
    resultLDR = One(ValueLDR, 200, 700);       // สว่างเกิน ต้องเปิด = 1 คือเปิด
    //Serial.println("resultLDR :" + String(resultLDR));
    //0.28-0.30
    if (resultLDR > 0.28) {
      resultLDR = 1;
    }
  }
  else {
    resultLDR = Zero(ValueLDR, 800, 1024);    // มืดเกิน ไม่ต้องเปิด = 0 คือไม่เปิด
    //Serial.println("resultLDR :" + String(resultLDR));
    //0.50-0.60
    if (resultLDR > 0.50) {
      resultLDR = 0;
    }
  }

  //Serial.println("resultLDR :" + String(resultLDR));

  if (Value_Moisture_percentage < 90) {
    resultSoil = One(Value_Moisture_percentage, 0, 100); // น้ำน้อยความชิ้นน้อย ต้องเปิด = 1 คือเปิด
    // Serial.println("resultSoil :" + String(resultSoil));
    //0.00
    if (resultSoil < 0.05) {
      resultSoil = 1;
    }
  }
  else {
    resultSoil = Zero(Value_Moisture_percentage, 90, 100); // น้ำมากความชื้นมากไม่ต้องเปิด  = 0 คือไม่เปิด
    //Serial.println("resultSoil :" + String(resultSoil));
    //0.30
    if (resultSoil > 0.25) {
      resultSoil = 0;
    }
  }

  //Serial.println("resultSoil :" + String(resultSoil));
  Averagewater_ALL = average_water(resultTemp, resultLDR, resultSoil);
  return Averagewater_ALL;
  //1.00

}
double Fan(float Value_Temperature, float Value_Moisture_percentage) {

  float resultTemp = 0;
  float resultSoil = 0;
  float Averagefan_ALL;

  if (Value_Temperature >= 30) {
    resultTemp = One(Value_Temperature, 30, 40); // ร้อนมาก = 1 คือเปิด
    //Serial.println("resultTemp :" + String(resultTemp));
    //0.31
    if (resultTemp > 0.30) {
      resultTemp = 1;
    }
  }
  else {
    resultTemp = Zero(Value_Temperature, 0, 30); // ไม่ร้อน = 0 คือปิด
    //Serial.println("resultTemp :" + String(resultTemp));
    //0.19
    if (resultTemp < 0.20) {
      resultTemp = 0;
    }
  }

  if (Value_Moisture_percentage > 90) {
    resultSoil = One(Value_Moisture_percentage, 90, 100); // น้ำมากความชิ้นมาก = 1 คือเปิด
    //Serial.println("resultSoil :" + String(resultSoil));
    //0.70
    if (resultSoil > 0.60) {
      resultSoil = 1;
    }
  }
  else {
    resultSoil = Zero(Value_Moisture_percentage, 0, 90); // น้ำน้อยความชิ้นน้อย = 0 คือปิด
    //Serial.println("resultSoil :" + String(resultSoil));
    //1.00
    if (resultSoil < 1.20) {
      resultSoil = 0;
    }
  }

  //Serial.println("resultSoil :" + String(resultSoil));

  Averagefan_ALL = average_fan(resultTemp, resultSoil);
  return Averagefan_ALL;
  //0.55
}

double Windows(int ValueLDR, float Value_Moisture_percentage)  {

  float resultLDR = 0;
  float resultSoil = 0;
  float Averagewindows_ALL;

  if (ValueLDR > 800) {
    resultLDR = One(ValueLDR, 800, 1024); // แสงน้อย มือ = 1 คือเปิด
    //Serial.println("resultLDR :" + String(resultLDR));
    //0.40
    if (resultLDR < 1.50) {
      resultLDR = 1;
    }
  } else {
    resultLDR = Zero(ValueLDR, 500, 800); // แสงมาก สว่าง = 0 คือปิด
    //Serial.println("resultLDR :" + String(resultLDR));
    //1.00
    if (resultLDR > 1.60) {
      resultLDR = 0;
    }
  }

  //Serial.println("resultLDR :" + String(resultLDR));

  if (Value_Moisture_percentage < 90) {
    resultSoil = One(Value_Moisture_percentage, 0, 100); // น้ำน้อยความชิ้นน้อย ต้องเปิด = 1 คือเปิด
    //Serial.println("resultSoil :" + String(resultSoil));
    //0.00
    if (resultSoil < 0.05) {
      resultSoil = 1;
    }
  }
  else {
    resultSoil = Zero(Value_Moisture_percentage, 90, 100); // น้ำมากความชื้นมากไม่ต้องเปิด  = 0 คือไม่เปิด
    //Serial.println("resultSoil :" + String(resultSoil));
    //0.30
    if (resultSoil > 0.25) {
      resultSoil = 0;
    }
  }
  //Serial.println("resultSoil :" + String(resultSoil));
  Averagewindows_ALL = average_windows(resultLDR, resultSoil);
  return Averagewindows_ALL;
  //1.00

}
float temperature() {
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);
  delay(1000);
  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);
  for ( i = 0; i < 9; i++) {
    data[i] = ds.read();
  }
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3;
    if (data[7] == 0x10) {
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;
    else if (cfg == 0x20) raw = raw & ~3;
    else if (cfg == 0x40) raw = raw & ~1;
  }
  celsius = (float)raw / 16.0;
  return celsius;
}
