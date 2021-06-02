#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>

#define RELAY_PIN1 4
#define RELAY_PIN2 5

// Config Firebase
#define FIREBASE_HOST "ggez-knmiei.firebaseio.com"
#define FIREBASE_AUTH "zRDJXT4ZATykXk7bzaAzQvv09H1PNPXoD0lOh8q7"

// Config connect WiFi
#define WIFI_SSID   "dlink-ABD0"
#define WIFI_PASS   "yyafr68490"

void setup() {

  Serial.begin(115200);
  pinMode(RELAY_PIN1, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);

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

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}
void loop() {

  if (Firebase.success()) {
    int room1_led1 = Firebase.getInt("room/room1/led1");
    Serial.println("ROOM1_LED1:" + String(room1_led1));

    int room1_led2 = Firebase.getInt("room/room1/led2");
    Serial.println("ROOM1_LED2:" + String(room1_led2));
    
    digitalWrite(RELAY_PIN1, (room1_led1 == 0 ? LOW : HIGH));
    digitalWrite(RELAY_PIN2, (room1_led2 == 0 ? LOW : HIGH));
  }

  if (Firebase.failed()) {
    Serial.println("streaming failed");
    Serial.println(Firebase.error());
  }
  delay(500);
}
