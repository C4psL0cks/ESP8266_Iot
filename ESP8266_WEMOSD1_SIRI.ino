#include <time.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial
#define BLYNK_MAX_SENDBYTES 256

#define LED1 D1
#define LED2 D2
#define relaychanal1 D3
#define relaychanal2 D4

#define BLYNK_TOKEN "c8ffe0bf829d4e13a15c8430f2ff70e2"
#define WIFI_SSID "6021607"
#define WIFI_PASS "17401449"


/// NTP time
time_t now;
struct tm* p_tm;
char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";


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
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
}
BLYNK_WRITE(V0) {
  buttonchanal1 = param.asInt();
  //Serial.println(buttonchanal1);
  if (buttonchanal1 == 0) {
    digitalWrite(relaychanal1, LOW);
    Blynk.notify("LED ON {DEVICE_NAME}");
    led1.on();
    digitalWrite(LED1, HIGH);
  }
  else if (buttonchanal1 == 1) {
    digitalWrite(relaychanal1, HIGH);
    Blynk.notify("LED OFF {DEVICE_NAME}");
    led1.off();
    digitalWrite(LED1, LOW);
  }
}
BLYNK_WRITE(V1) {
  buttonchanal2 = param.asInt();
  //Serial.println(buttonchanal2);
  if (buttonchanal2 == 0) {
    digitalWrite(relaychanal2, LOW);
    led2.on();
    digitalWrite(LED2, HIGH);
  }
  else if (buttonchanal2 == 1) {
    digitalWrite(relaychanal2, HIGH);
    led2.off();
    digitalWrite(LED2, LOW);
  }
}
void setup() {

  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(relaychanal1, OUTPUT);
  pinMode(relaychanal2, OUTPUT);
  digitalWrite(relaychanal1, HIGH);
  digitalWrite(relaychanal2, HIGH);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  led1.off();
  led2.off();
  if (strlen(BLYNK_TOKEN) == 0) {
    BLYNK_ENABLED = false;
  }
  Blynk.begin(BLYNK_TOKEN, WIFI_SSID, WIFI_PASS);

  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
}

void loop() {

  now = time(nullptr);
  p_tm = localtime(&now);

  if (BLYNK_ENABLED) {
    if (Blynk.connected()) {
      Blynk.run();
    }
  }

  Blynk.virtualWrite(V0, buttonchanal1);
  Blynk.virtualWrite(V1, buttonchanal2);
  Blynk.virtualWrite(V5, ctime(&now));
  int sensorValue = ESP.getVcc();
  float volts = (float)sensorValue / 1023.00;
  Blynk.virtualWrite(V4, volts);

}
