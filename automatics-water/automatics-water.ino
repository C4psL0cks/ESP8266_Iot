#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define WIFI_SSID   "6021607"
#define WIFI_PASS   "17401449"
#define FIREBASE_HOST "automatics-hamako.firebaseio.com"
#define FIREBASE_AUTH "yo9a6i3xDI8k1b1pYsMeSq0h4q4WKzQUh30JXLQh"

int moisture_value;
#define relay_Pin  D1

void setup() {
  Serial.begin(115200);
  pinMode(relay_Pin, OUTPUT);
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
}

void loop() {

  /// sensor
  Serial.print("MOISTURE LEVEL : ");
  moisture_value = analogRead(A0);
  moisture_value /=  10;
  Serial.println(moisture_value);

  Firebase.setInt("sensor/values_sensor", moisture_value);

  int state_relay = Firebase.getInt("sensor/relay/");
  //  Serial.println(state_relay);
  digitalWrite(relay_Pin, (state_relay == 0 ? HIGH : LOW));

  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /message failed:");
    Serial.println(Firebase.error());
    return;
  }

  delay(1000);
}
