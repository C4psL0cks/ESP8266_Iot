#include <time.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <TridentTD_LineNotify.h>
#include "DHT.h"

#define DHTPIN D2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define FIREBASE_HOST "xxxx-xxxx.firebaseio.com"
#define FIREBASE_AUTH "xxxx"
#define WIFI_SSID   "dlink-ABD0"
#define WIFI_PASS   "yyafr68490"

#define LINE_TOKEN "xxxxx"

/// time
struct tm* p_tm;
char ntp_server1[20] = "ntp.ku.ac.th";
char ntp_server2[20] = "fw.eng.ku.ac.th";
char ntp_server3[20] = "time.uni.net.th";
int sec;
String times = "";

/// Presser
int sensorPin = A0, sensorValue = 0, offset = 0;
float Vout = 0, v2;
int s2, i = 0, sum = 0;

/// dht22
float Humidity, Temperature, Pressure;
String roomName = "ICU(1)";
bool checkHumidity = false;
bool checkTemperature = false;
bool checkPressure = false;

void setup() {

  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("Running!");
  Serial.println("-------------------------------------");
  // Connect to Wifi.
  Serial.println();
  WiFi.mode(WIFI_STA);// connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("WiFi Connecting to %s\n", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  LINE.setToken(LINE_TOKEN);

  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  LINE.notify("รอการเชื่อมต่อสักครู่...");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  dht.begin();

  Serial.println("init...");
  //Calibrate offeset
  for (i = 0; i < 10; i++) {
    sensorValue = analogRead(sensorPin) - 512; //How is this value chosen? should this be 2048??
    sum += sensorValue;
  }
  offset = sum / 10.0;
  Serial.println("Ok");

}

void loop() {

  time_t time_now = time(nullptr);   // Time
  p_tm = localtime(&time_now);
  sec = p_tm->tm_min; //tm_min
  times = ctime(&time_now);

  // DHT22
  Humidity = dht.readHumidity();
  Temperature = dht.readTemperature();

  if (isnan(Humidity) || isnan(Temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  sensorValue = analogRead(sensorPin);
  s2 = sensorValue - offset;
  v2 = (5 * s2) / 1024.0;
  Pressure = v2 - 2.5;
  Pressure = Pressure * 1000;

  // show string value
  Serial.println("TimeNow:" + String(sec));
  Serial.println("Humidity: " + String(Humidity) + "%");
  Serial.println("Temperature: " + String(Temperature) + "°C");
  Serial.println("Presure : " + String(Pressure) + "Pa");

  //set string value
  Firebase.setString(roomName + "/Humidity", String(Humidity));
  Firebase.setString(roomName + "/Temperature", String(Temperature));
  Firebase.setString(roomName + "/Pressure", String(Pressure));
  Firebase.setString(roomName + "/times", String(times));
  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /message failed:");
    Serial.println(Firebase.error());
    return;
  }

  if ((sec % 5) == 0) {

    Serial.println("CHECK:" + String(sec));
    if (Humidity <= 45 && Humidity >= 60  && checkHumidity == false) {
      LINE.notify(roomName + "\nความชื้น เกินกว่าเกณฑ์: " + Humidity + "%");
      checkHumidity = true;
    }
    if (Temperature >= 25 && checkTemperature == false) {
      LINE.notify(roomName + "\nอุณหภูมิ เกินกำหนดที่: " + Temperature + "°C");
      checkTemperature = true;
    }
    if (Pressure >= -2.5 && checkPressure == false) {
      LINE.notify(roomName + "\nความดัน เกินกำหนดที่: " + Pressure + "Pa");
      checkPressure = true;
    }

  }
  else {
    Serial.println("RESRT:" + String(sec));
    checkHumidity = false;
    checkTemperature = false;
    checkPressure = false;
  }
}
