#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial
#define BLYNK_MAX_SENDBYTES 256

#define relaychanal1 D3
#define relaychanal2 D4

#define BLYNK_TOKEN "595f31446d1f4567b77d2d425b9011ea"
#define WIFI_SSID "6021607"
#define WIFI_PASS "17401449"

ADC_MODE(ADC_VCC);
static bool BLYNK_ENABLED = true;
int buttonchanal1, buttonchanal2;

WidgetLED led1(V2);
WidgetLED led2(V3);

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
}
BLYNK_WRITE(V0) {
  buttonchanal1 = param.asInt();
  //Serial.println(buttonchanal1);
  if (buttonchanal1 == 0) {
    digitalWrite(relaychanal1, LOW);
    Blynk.notify("LED ON {DEVICE_NAME}");
    led1.on();
  }
  else if (buttonchanal1 == 1) {
    digitalWrite(relaychanal1, HIGH);
    Blynk.notify("LED OFF {DEVICE_NAME}");
    led1.off();
  }
}
BLYNK_WRITE(V1) {
  buttonchanal2 = param.asInt();
  //Serial.println(buttonchanal2);
  if (buttonchanal2 == 0) {
    digitalWrite(relaychanal2, LOW);
    //Blynk.notify("LED ON {DEVICE_NAME}");
    led2.on();
  }
  else if (buttonchanal2 == 1) {
    digitalWrite(relaychanal2, HIGH);
    //Blynk.notify("LED OFF {DEVICE_NAME}");
    led2.off();
  }
}
void setup() {

  Serial.begin(115200);
  pinMode(relaychanal1, HIGH);
  pinMode(relaychanal2, HIGH);
  led1.off();
  led2.off();
  if (strlen(BLYNK_TOKEN) == 0) {
    BLYNK_ENABLED = false;
  }
  Blynk.begin(BLYNK_TOKEN, WIFI_SSID, WIFI_PASS, "blynk-cloud.com", 8442);
}
void loop() {

  if (BLYNK_ENABLED) {
    if (Blynk.connected()) {
      Blynk.run();
    }
  }
  Blynk.virtualWrite(V0, buttonchanal1);
  Blynk.virtualWrite(V1, buttonchanal2);
  int sensorValue = ESP.getVcc();
  float volts = (float)sensorValue / 1023.00;
  Blynk.virtualWrite(V4, volts);
}
