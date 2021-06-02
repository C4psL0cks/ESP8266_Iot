#include <time.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define WIFI_SSID   "dlink-ABD0"
#define WIFI_PASS   "yyafr68490"
#define FIREBASE_HOST "arunsawad-5f45d.firebaseio.com"
#define FIREBASE_AUTH "Rk6fRjLB2Kwt77tf8rCgTHfj9lt6SwM17QPv9P3u"


String Humidity;
String Temperature;
String Pressure;
String times = "";
String roomName = "ICU(1)";

void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("Running!");
  Serial.println("-------------------------------------");
  // Connect to Wifi.
  Serial.println();
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
}

void loop() {


  Humidity = Firebase.getString(roomName + "/Humidity");
  Temperature = Firebase.getString(roomName + "/Temperature");
  Pressure = Firebase.getString(roomName + "/Pressure");
  times = Firebase.getString(roomName + "/times");

  if (Firebase.failed()) {
    Serial.println(Firebase.error());
    return;
  } else {
    // show string value
    Serial.println("Humidity: " + String(Humidity) + "%");
    Serial.println("Temperature: " + String(Temperature) + "°C");
    Serial.println("Presure : " + String(Pressure) + "Pa");
    Serial.println("times : " + String(times));
  }
  delay(1000);
}
