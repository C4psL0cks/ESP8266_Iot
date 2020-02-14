#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>

#define LED_PIN 4

// Config Firebase
#define FIREBASE_HOST "database-ionics.firebaseio.com"
#define FIREBASE_AUTH "RX91Pa9yoE5UNa50tPUjPOGT3O0A9WOv9ZFLntpX"

// Config connect WiFi
#define WIFI_SSID "6021607"
#define WIFI_PASSWORD "17401449"
int state;

void setup() {
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);

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
  Firebase.stream("/switch/switch1");
}

void loop() {


  if (Firebase.success()) {
    Serial.println("streaming success");
    if (Firebase.available()) {
      FirebaseObject event = Firebase.readEvent();
      int data = event.getInt("data");
      Serial.println(data);
      state = data ? 1 : 0;
    }
  }
  if (state) {
    Serial.println("STATE RUN 1");
    digitalWrite(LED_PIN, HIGH);
  }
  else {
    Serial.println("STATE RUN 0");
    digitalWrite(LED_PIN, LOW);
  }

  if (Firebase.failed()) {
    Serial.println("streaming failed");
    Serial.println(Firebase.error());
  }

  delay(1000);
}
