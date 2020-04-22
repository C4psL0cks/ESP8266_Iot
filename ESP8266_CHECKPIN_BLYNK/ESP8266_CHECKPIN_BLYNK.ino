#define BLYNK_MAX_SENDBYTES 256
#define BLYNK_PRINT Serial 
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

int buttonState;
int chanal1 = 20;
int bt1 = 16;

char auth[] = "xxxxxx";
#define WIFI_SSID  "6021607"
#define WIFI_PASS  "17401449"

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, WIFI_SSID, WIFI_PASS, "blynk-cloud.com", 8442);
  pinMode(bt1, OUTPUT);
  digitalWrite(bt1, LOW);
}

BLYNK_WRITE(V5) {
  buttonState = param.asInt();
  if (buttonState == 1) {
    chanal1 = 20;
  }
}

void loop() {

  Blynk.run();
  if (digitalRead(bt1) == HIGH) {
    if (digitalRead(bt1) == HIGH) {
      chanal1 -= 1;
      if (chanal1 < 0) {
        chanal1 = 0;
      }
      Serial.println("BT1 : ON");
      Serial.println(chanal1);
    }
  }
  Blynk.virtualWrite(1, chanal1);

}
