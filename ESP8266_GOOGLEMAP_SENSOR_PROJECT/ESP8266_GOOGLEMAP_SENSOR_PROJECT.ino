#include <time.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>

#include "DHT.h"
#define DHTPIN 14
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#include <BH1750FVI.h>
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

#define WIFI_SSID "dlink-ABD0"
#define WIFI_PASS "yyafr68490"

#define FIREBASE_HOST "smart-light-4abfa.firebaseio.com"
#define FIREBASE_AUTH "5QpQeZTYBvt4JCeJmv2BlG0KmGpq3MrdFwMRRSxJ"

/// time
struct tm* p_tm;
char ntp_server1[20] = "ntp.ku.ac.th";
char ntp_server2[20] = "fw.eng.ku.ac.th";
char ntp_server3[20] = "time.uni.net.th";
int sec;
String times = "";

bool checkHumidity = false;
bool checkTemperature = false;
bool checkTemperature_outtime = false;
bool checkHumidity_outtime = false;
bool checkremoves = false;

int alarm_id = 0;
String device_id = "001";

void setup() {
  Serial.begin(115200);
  // Start the DHT22 sensor
  dht.begin();
  LightSensor.begin();
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("Running!");
  Serial.println("-------------------------------------");
  // Connect to Wifi.
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("WiFi Connecting to %s\n", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);//connect to firebase

  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3); // connect ntp server clock
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }

}

void loop() {

  StaticJsonBuffer<300> JSONbuffer;

  time_t time_now = time(nullptr);   // Time
  p_tm = localtime(&time_now);
  sec = p_tm->tm_min; //tm_min
  int sec_s = p_tm->tm_hour; //tm_min
  times = ctime(&time_now);

  // Light
  uint16_t lux = LightSensor.GetLightIntensity();

  // DHT22
  float Humidity = dht.readHumidity();
  float Temperature = dht.readTemperature();

  if (isnan(Humidity) || isnan(Temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // show string value
  Serial.println("TimeNow:" + String(times));
  Serial.println("Light: " + String(lux));
  Serial.println("Humidity: " + String(Humidity) + "%");
  Serial.println("Temperature: " + String(Temperature) + "°C");

  /// sensor
  Firebase.setString("sensor_value/logDHT/device_id", device_id);
  Firebase.setInt("sensor_value/logDHT/value_light", lux);
  Firebase.setFloat("sensor_value/logDHT/value_temperature", Temperature);
  Firebase.setFloat("sensor_value/logDHT/value_humidity", Humidity);
  Firebase.setString("sensor_value/logDHT/value_time", times);

  // set float value
  Firebase.setString("devices/0/device_status", "actives");
  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /message failed:");
    Serial.println(Firebase.error());
    return;
  }

  delay(3000);
}
