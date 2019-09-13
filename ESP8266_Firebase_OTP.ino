#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set these to run example.
#define FIREBASE_HOST "espdemo-6518c.firebaseio.com"
#define FIREBASE_AUTH "YhIZkV7dc8UGALQFalhwxRjqbihIDCM9SVcQoN2s"
#define WIFI_SSID "Jomson"
#define WIFI_PASSWORD "jomsonc007"
String getuid = "xxxx";
int OTP, OTPS;
String token = "111";
int state = false;

String chipid = "";
String value = "";
String values = "";
String battery = "90";
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

  chipid = ESP.getChipId();
  Firebase.setString("/device/" + chipid + "/battery", battery);
  value = Firebase.getString("/device/" + chipid + "/lastunlock");
  if (values == "") {
    values = value;
  }
  if (value != values) {

    state = !state;
    values = value;
  }
  Serial.println(state);
  if (Firebase.failed()) {
    Serial.print("GET / failed:");
    Serial.println(Firebase.error());
    return;
  }
  
  //  if (Firebase.getString("/user/token") == token) {
  //    state = true;
  //  }
  //  if (state == true) {
  //    //RFID();
  //    Serial.println("OTP OK");
  //  }
  //  if (Firebase.getString("/user/" + getuid + "/uid") == getuid) {
  //    Serial.println(Firebase.getString("/user/" + uid));
  //    Firebase.setInt("/user/" + uid + "/OTP", ESP.getChipId());
  //    state = true;
  //  }
  //  if (state == true) {
  //    if (Firebase.getInt("/user/" + uid + "/OTP") == Firebase.getInt("/user/" + uid + "/OTPS")) {
  //      Serial.println("OTP OK");
  //    }
  //    else {
  //      Serial.println("OTP unknow");
  //    }
  //  }
  //  if (Firebase.failed()) {
  //    Serial.print("GET / failed:");
  //    Serial.println(Firebase.error());
  //    return;
  //  }
}
