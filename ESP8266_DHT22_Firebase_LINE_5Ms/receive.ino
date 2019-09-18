#include <time.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "arunsawad-5f45d.firebaseio.com"
#define FIREBASE_AUTH "Rk6fRjLB2Kwt77tf8rCgTHfj9lt6SwM17QPv9P3u"
#define WIFI_SSID "6021607"
#define WIFI_PASSWORD "17401449"

String Humidity;
String Temperature;
String Pressure;
String times = "";
String roomName = "ICU(1)";

void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);// connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

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
