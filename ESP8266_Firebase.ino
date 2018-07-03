#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>

// Config Firebase
#define FIREBASE_HOST "xxx"
#define FIREBASE_AUTH "xxx"

// Config connect WiFi
#define WIFI_SSID "xxx"
#define WIFI_PASSWORD "xxx"


const int LED_PIN = D6;
const int FAN_PIN = D7;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(WiFi.localIP());
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.setInt("Control/LED", 0);  // led 0 - 1
  Firebase.setInt("Control/FAN", 0); // fan 0 - 0

}

void loop() {
  // put your main code here, to run repeatedly:
  //1 = HIGH
  //0 = LOW
  //digitalWrite(LED_PIN,HIGH); //1
  //digitalWrite(LED_PIN,LOW); //0
  //Firebase.getInt("Control/LED") = 1
  //digitalWrite(LED_PIN,1)
  //digitalWrite(LED_PIN,HIGH); //1
  
  digitalWrite(LED_PIN, Firebase.getInt("Control/LED"));
  digitalWrite(FAN_PIN, Firebase.getInt("Control/FAN"));
  delay(200);
}