#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set these to run example.
#define FIREBASE_HOST "xx"
#define FIREBASE_AUTH "xx"
#define WIFI_SSID "xxxxx"
#define WIFI_PASSWORD "xxxx"
String getuid = "xxxx";
int OTP, OTPS;
int state = false;
void setup() {
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

  Serial.printf(" ESP8266 Chip id = %08X\n", ESP.getChipId());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}
void loop() {

  if (Firebase.getString("/user/" + getuid + "/uid") == getuid) {
    Serial.println(Firebase.getString("/user/" + uid));
    Firebase.setInt("/user/" + uid + "/OTP", ESP.getChipId());
    state = true;
  }
  if (state == true) {
    if (Firebase.getInt("/user/" + uid + "/OTP") == Firebase.getInt("/user/" + uid + "/OTPS")) {
      Serial.println("OTP OK");
    }
    else {
      Serial.println("OTP unknow");
    }
  }
  if (Firebase.failed()) {
    Serial.print("GET / failed:");
    Serial.println(Firebase.error());
    return;
  }
}
