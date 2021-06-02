#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include <TridentTD_LineNotify.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT22
#define RELAY_PIN1 5
#define analogPin A0
#define FIREBASE_HOST "water-b5fc4-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "bGpEOLvK1WLIk9AvWC3KYZ3S4eRYXBNA4qYm6St2"
#define WIFI_SSID   "dlink-ABD0"
#define WIFI_PASS   "yyafr68490"
//#define LINE_TOKEN "VOwca6uoY6T5vR6A43J7zwg2PtOWG2z4wGvgTLFsN3u"

struct tm* p_tm;
char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";
String times = "";
int Minutes;
int Hours;
float Humidity;
float Temperature;
bool checkHumidity = false;
bool checkbt = false;

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(115200);
  dht.begin();
  pinMode(RELAY_PIN1, OUTPUT);
  digitalWrite(RELAY_PIN1, HIGH);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  // connect to ntptime.
  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  LINE.notify("รอการเชื่อมต่อเวลาสักครู่...สำเร็จแล้ว");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }

  // connect to firebase.
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  String LINE_TOKEN = Firebase.getString("linekey/");
  delay(1000);

  // connect to line.
  LINE.setToken(LINE_TOKEN);

}
void loop() {

  // get time
  time_t time_now = time(nullptr);
  p_tm = localtime(&time_now);
  Minutes = p_tm->tm_min;
  Hours = p_tm->tm_hour;
  times = ctime(&time_now);
  Humidity = map(analogRead(analogPin), 1024, 0, 0, 100);
  Temperature = dht.readTemperature();

  // check sensor dht22
  if (isnan(Temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.println("=======================================");
  Serial.println("Times:" + String(times));
  Serial.println("Temperature:" + String(Temperature) + " *C");
  Serial.println("Humidity: " + String(Humidity) + "%");
  Serial.println("=======================================");

  // check send notification 5 minutes
  if (Minutes % 5 == 0) {
    Serial.println("=======================================");
    Serial.println("ALERT 5 Minutes:" + String(Minutes));
    Serial.println("=======================================");

    if (Humidity >= 50 && checkHumidity == false) {
      LINE.notify("\nความชื้น ต่ำกว่าเกณฑ์: " + String(Humidity) + "%");
      checkHumidity = true;
    }
  }
  else {
    Serial.println("=======================================");
    Serial.println("RESRT 5 Minutes :" + String(Minutes));
    Serial.println("=======================================");
    checkHumidity = false;
  }

  // get value form firebase database
  int pum = Firebase.getInt("pum/status");
  int autos = Firebase.getInt("pum/auto");
  String timestart = Firebase.getString("pum/timestart");
  String timesend = Firebase.getString("pum/timeend");
  if (Firebase.failed()) {
    Serial.println("streaming failed");
    Serial.println(Firebase.error());
  }

  // check mode auto manual
  if (autos == 0) {
    Serial.println("Autos: OFF");
    if (pum == 1) {
      Serial.println("=======================================");
      Serial.println("PumON");
      Serial.println("=======================================");
      PumON();
    }
    else {
      Serial.println("=======================================");
      Serial.println("PumOff");
      Serial.println("=======================================");
      PumOff();
    }
  }
  if (autos == 1) {
    Serial.println("Autos: ON");
    Serial.println("Autos:" + String(autos));
    Serial.println("Hours:" + String(Hours));
    Serial.println("Timestart:" + String(timestart));
    Serial.println("Timesend:" + String(timesend));
    if (String(Hours) == timestart) {
      Serial.println("=======================================");
      Serial.println("PumON");
      Serial.println("=======================================");
      PumON();
    }
    if (String(Hours) == timesend) {
      Serial.println("=======================================");
      Serial.println("PumOff");
      Serial.println("=======================================");
      PumOff();
    }
  }
  delay(1000);
}

void PumON() {
  digitalWrite(RELAY_PIN1, LOW);
}
void PumOff() {
  digitalWrite(RELAY_PIN1, HIGH);
}
