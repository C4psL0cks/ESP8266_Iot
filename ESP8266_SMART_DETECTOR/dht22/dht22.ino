#include <time.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <TridentTD_LineNotify.h>
#include <ArduinoJson.h>

#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define WIFI_SSID   "6021607"
#define WIFI_PASS   "17401449"
#define FIREBASE_HOST "smartdetector-1954c.firebaseio.com"
#define FIREBASE_AUTH "Gy0QrO6mByhon4okSQdUaRAqKiJxsyi8kbiAZ2uw"
#define LINE_TOKEN "cJwwtx0gT5zuZGwzCWtdK7SZXHPZxJzGCrcchxUqWxT"

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
  LINE.setToken(LINE_TOKEN);//connect to line

  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3); // connect ntp server clock
  Serial.println("\nWaiting for time");
  LINE.notify("รอการเชื่อมต่อจาก DHT22 สักครู่...");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  LINE.notify("เชื่อมต่อสำเร็จ...DHT22");
}

void loop() {

  time_t time_now = time(nullptr);   // Time
  p_tm = localtime(&time_now);
  sec = p_tm->tm_min; //tm_min
  int sec_s = p_tm->tm_hour; //tm_min
  times = ctime(&time_now);

  // DHT22
  float Humidity = 32;
  float Temperature = 32;

  if (isnan(Humidity) || isnan(Temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // show string value
  Serial.println("TimeNow:" + String(times));
  Serial.println("Humidity: " + String(Humidity) + "%");
  Serial.println("Temperature: " + String(Temperature) + "°C");

  StaticJsonBuffer<300> JSONbuffer;

  /// data
  JsonObject& data = JSONbuffer.createObject();
  data["device_id"] = device_id;
  data["value_temperature"] = Temperature;
  data["value_humidity"] = Humidity;
  data["value_time"] = times;

  Firebase.push("data/logDHT", data);
  // handle error
  if (Firebase.failed()) {
    Serial.print("pushing /logs failed:");
    Serial.println(Firebase.error());
    return;
  }

  /// sensor
  Firebase.setString("sensor_value/logDHT/device_id", device_id);
  Firebase.setFloat("sensor_value/logDHT/value_temperature", Temperature);
  Firebase.setFloat("sensor_value/logDHT/value_humidity", Humidity);
  Firebase.setString("sensor_value/logDHT/value_time", times);

  // handle error
  if (Firebase.failed()) {
    Serial.print("set /logs failed:");
    Serial.println(Firebase.error());
    return;
  }

  // set float value
  Firebase.setString("devices/0/device_status", "actives");
  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /message failed:");
    Serial.println(Firebase.error());
    return;
  }

  // in time
  Serial.println("TIME:" + String(sec_s));
  if (sec_s == 8 || sec_s == 16) {
    if ((sec % 5) == 0) {
      if ((sec % 5) == 0 && checkremoves == false) {
        Firebase.remove("data/logDHT");
        checkremoves = true;
      }
      Serial.println("SEND_IN_TIME:" + String(sec));
      if (Temperature > 30  && checkTemperature == false) {
        LINE.notify("\nอุณหภูมิ ในเวลาช่วง 08.00-16.00 เกินค่าที่กำหนด: " + String(Temperature) + "°C จาก sensor DHT22");
        StaticJsonBuffer<300> JSONbuffer;
        JsonObject& alarm = JSONbuffer.createObject();
        alarm["alarm_id"] = alarm_id++;
        alarm["alarm_device_id"] = device_id;
        alarm["alarm_detail"] = "อุณหภูมิ ในเวลาช่วง 08.00-16.00 เกินค่าที่กำหนด: " + String(Temperature) + "°C จาก sensor DHT22";
        alarm["alarm_time"] = times;
        Firebase.push("alarms/in_time/", alarm);
        // handle error
        if (Firebase.failed()) {
          Serial.print("pushing /logs failed:");
          Serial.println(Firebase.error());
          return;
        }
        delay(500);
        checkTemperature = true;
      }
      // 45 - Humidity -60
      if (Humidity > 60  && checkHumidity == false) {
        LINE.notify("\nความชื้น ในเวลาช่วง 08.00-16.00 เกินค่าที่กำหนด: " + String(Humidity) + "% จาก sensor DHT22");
        StaticJsonBuffer<300> JSONbuffer;
        JsonObject& alarm = JSONbuffer.createObject();
        alarm["alarm_id"] = alarm_id++;
        alarm["alarm_device_id"] = device_id;
        alarm["alarm_detail"] = "ความชื้น ในเวลาช่วง 08.00-16.00 เกินค่าที่กำหนด: " + String(Humidity) + "% จาก sensor DHT22";
        alarm["alarm_time"] = times;
        Firebase.push("alarms/in_time/", alarm);
        // handle error
        if (Firebase.failed()) {
          Serial.print("pushing /logs failed:");
          Serial.println(Firebase.error());
          return;
        }
        delay(500);
        checkHumidity = true;
      }
    }
    else {
      Serial.println("RESRT_IN_TIME:" + String(sec));
      checkTemperature = false;
      checkHumidity = false;
      checkremoves = false;
    }
  }
  // outtime
  if ((sec % 3) == 0) {
    Serial.println("SEND_OUT_TIME:" + String(sec));
    if (Temperature > 30 && checkTemperature_outtime == false) {
      LINE.notify("\nอุณหภูมิ นอกเวลาช่วง 08.00-16.00 เกินค่าที่กำหนด: " + String(Temperature) + "°C จาก sensor DHT22");
      StaticJsonBuffer<300> JSONbuffer;
      JsonObject& alarm = JSONbuffer.createObject();
      alarm["alarm_id"] = alarm_id++;
      alarm["alarm_device_id"] = device_id;
      alarm["alarm_detail"] = "อุณหภูมิ นอกเวลาช่วง 08.00-16.00 เกินค่าที่กำหนด: " + String(Temperature) + "°C จาก sensor DHT22";
      alarm["alarm_time"] = times;
      Firebase.push("alarms/out_time/", alarm);
      // handle error
      if (Firebase.failed()) {
        Serial.print("pushing /logs failed:");
        Serial.println(Firebase.error());
        return;
      }
      delay(500);
      checkTemperature_outtime = true;
    }
    // 45 - Humidity -60
    if (Humidity > 70  && checkHumidity_outtime == false) {
      LINE.notify("\nความชื้น นอกเวลาช่วง 08.00-16.00 เกินค่าที่กำหนด: " + String(Humidity) + "% จาก sensor DHT22");
      StaticJsonBuffer<300> JSONbuffer;
      JsonObject& alarm = JSONbuffer.createObject();
      alarm["alarm_id"] = alarm_id++;
      alarm["alarm_device_id"] = device_id;
      alarm["alarm_detail"] = "ความชื้น นอกเวลาช่วง 08.00-16.00 เกินค่าที่กำหนด: " + String(Humidity) + "% จาก sensor DHT22";
      alarm["alarm_time"] = times;
      Firebase.push("alarms/out_time/", alarm);
      // handle error
      if (Firebase.failed()) {
        Serial.print("pushing /logs failed:");
        Serial.println(Firebase.error());
        return;
      }
      delay(500);
      checkHumidity_outtime = true;
    }
  }
  else {
    Serial.println("RESRT_OUT_TIMR:" + String(sec));
    checkTemperature_outtime = false;
    checkHumidity_outtime = false;
  }
  delay(1000);
}
