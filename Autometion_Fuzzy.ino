#include <ESP8266WiFi.h>
#include <time.h>

#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#include <MCP3008.h>
#define CS_PIN D8
#define CLOCK_PIN D5
#define MOSI_PIN D7
#define MISO_PIN D6
MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);

#define WIFI_SSID "6021607"
#define WIFI_PASS  "17401449"

const int Sensor_Pin_Moisture = 0;
const int Sensor_Pin_LDR = 1;
int ValueLDR = 0;
float Value_Temperature = 0;
float Value_Moisture_percentage = 0;

double Fuzzy_Average_Water;
double Fuzzy_Average_Fan;
double Fuzzy_Average_Windows;

char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";
ADC_MODE(ADC_VCC);

double ResultTempFuzzy = 0;
double ResultLDRFuzzy = 0;
double ResultSoilFuzzy = 0;
double ResultAverage = 0;
double Fuzzy_Average;

void setup() {

  Serial.begin(115200);
  dht.begin();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
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

}

void loop() {

  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  //Serial.println(ctime(&now));

  int sensorValue = ESP.getVcc();
  float volts = (float)sensorValue / 1023.00;
  //Serial.println(volts);

  Value_Temperature = dht.readTemperature();

  if (isnan(Value_Temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  ValueLDR = adc.readADC(Sensor_Pin_LDR);

  Value_Moisture_percentage = ( 100.00 - ( (adc.readADC(Sensor_Pin_Moisture) / 1023.00) * 100.00 ) );

  Fuzzy_Average_Water = Water(Value_Temperature, ValueLDR, Value_Moisture_percentage); //Average water
  Serial.print("Fuzzy_Average_Water : " + String(Fuzzy_Average_Water));

  Fuzzy_Average_Fan = Fan(Value_Temperature, Value_Moisture_percentage); //Average Fan
  Serial.print(" Fuzzy_Average_Fan : " + String(Fuzzy_Average_Fan));

  Fuzzy_Average_Windows = Windows(ValueLDR, Value_Moisture_percentage); //Average windonws
  Serial.print(" Fuzzy_Average_Windows : " + String(Fuzzy_Average_Windows));

  Serial.println();

  Fuzzy_Average = (Fuzzy_Average_Water + Fuzzy_Average_Fan + Fuzzy_Average_Windows) / 3;
  Serial.println("Fuzzy_Average :" + String(Fuzzy_Average));


  delay(500);
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
    resultTemp = Zero(Value_Temperature, 27, 40);
  } else {
    resultTemp = One(Value_Temperature, 0, 27);
  }

  if (ValueLDR >= 200 && ValueLDR <= 400) {
    resultLDR = One(ValueLDR, 200, 400);
  } else if (ValueLDR >= 700 && ValueLDR <= 1024) {
    resultLDR = One(ValueLDR, 700, 1024);
  } else {
    resultLDR = Zero(ValueLDR, 0, 1024);
  }

  if (Value_Moisture_percentage >= 900) {
    resultSoil = One(Value_Moisture_percentage, 900, 1024);
  } else {
    resultSoil = Zero(Value_Moisture_percentage, 0, 899);
  }
  Averagewater_ALL = average_water(resultTemp, resultLDR, resultSoil);
  return Averagewater_ALL;
}


double Fan(float Value_Temperature, float Value_Moisture_percentage) {

  float resultTemp = 0;
  float resultSoil = 0;
  float Averagefan_ALL;

  if (Value_Temperature <= 30) {
    resultTemp = Zero(Value_Temperature, 0, 30);
  } else {
    resultTemp = One(Value_Temperature, 31, 42);
  }

  if (Value_Moisture_percentage >= 200) {
    resultSoil = One(Value_Moisture_percentage, 200, 1024);
  } else {
    resultSoil = Zero(Value_Moisture_percentage, 0, 199);
  }

  Averagefan_ALL = average_fan(resultTemp, resultSoil);
  return Averagefan_ALL;
}

double Windows(int ValueLDR, float Value_Moisture_percentage)  {

  float resultLDR = 0;
  float resultSoil = 0;
  float Averagewindows_ALL;

  if (ValueLDR <= 200) {
    resultLDR = One(ValueLDR, 0, 200);
  } else {
    resultLDR = Zero(ValueLDR, 201, 1024);
  }

  if (Value_Moisture_percentage <= 512) {
    resultSoil = One(Value_Moisture_percentage, 0, 512);
  } else {
    resultSoil = Zero(Value_Moisture_percentage, 513, 1024);
  }

  Averagewindows_ALL = average_windows(resultLDR, resultSoil);
  return Averagewindows_ALL;
}


