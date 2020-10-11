#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define R D5
#define G D6
#define B D7

#define FIREBASE_HOST "db-ionics-tester.firebaseio.com"
#define FIREBASE_AUTH "QoZE7t6nSIHXKSW7BqQQZvgLyXvhdow4oFKILM8d"
#define WIFI_SSID "dlink-ABD0"
#define WIFI_PASSWORD "yyafr68490"


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("Running!");
  Serial.println("-------------------------------------");
  // Connect to Wifi.
  Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("WiFi Connecting to %s\n", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  pinMode(R, OUTPUT); pinMode(G, OUTPUT); pinMode(B, OUTPUT);

}
void loop() {
  // run
  Serial.println(Firebase.getInt("/servo/Range1"));
  Serial.println(Firebase.getInt("/servo/Range2"));
  Serial.println(Firebase.getInt("/servo/Range3"));
  
  analogWrite(R, Firebase.getInt("/servo/Range1"));
  analogWrite(G, Firebase.getInt("/servo/Range2"));
  analogWrite(B, Firebase.getInt("/servo/Range3"));

}
