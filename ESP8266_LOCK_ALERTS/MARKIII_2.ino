#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial
#define BLYNK_MAX_SENDBYTES 256
#include <TridentTD_LineNotify.h>

#define LEDBLUE D1
#define LEDRED D2
#define relaychanal1 D3
#define relaychanal2 D4

#define BLYNK_TOKEN "3abf05994fea46f9a7ae1918e2442702"
#define LINE_TOKEN "wiN66datagETOi1HG1UZIqWLeOODgBg0u0dXtCDKe4C"
#define WIFI_SSID "6021607"
#define WIFI_PASS "17401449"

ADC_MODE(ADC_VCC);
static bool BLYNK_ENABLED = true;
bool closes = false;
bool opens = false;
int buttonchanal1, buttonchanal1s;
String namedevice = "DOOR2";
WidgetLED led1(V2); // blue
WidgetLED led2(V3); // red

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V10);
  Blynk.syncVirtual(V11);
  Blynk.syncVirtual(V12);
  Blynk.syncVirtual(V13);
  Blynk.syncVirtual(V14);
  Blynk.syncAll();
}
BLYNK_WRITE(V10)
{
  buttonchanal1 = param.asInt();
  Serial.println(buttonchanal1);
  if (buttonchanal1 == 0)
  {
    digitalWrite(relaychanal1, LOW);
    Blynk.notify(namedevice + " DOOR CLOSE");
    closes = true;
    digitalWrite(LEDBLUE, HIGH);
    digitalWrite(LEDRED, LOW);
    led1.on();
    led2.off();
  }
  else if (buttonchanal1 == 1)
  {
    digitalWrite(relaychanal1, HIGH);
    Blynk.notify(namedevice + " DOOR OPEN");
    opens = true;
    digitalWrite(LEDBLUE, LOW);
    digitalWrite(LEDRED, HIGH);
    led2.on();
    led1.off();
  }
}
BLYNK_WRITE(V14)
{
  buttonchanal1s = param.asInt();
  Serial.println(buttonchanal1s);
  if (buttonchanal1s == 0)
  {
    digitalWrite(relaychanal1, LOW);
    Blynk.notify(namedevice + " DOOR CLOSE");
    closes = true;
    digitalWrite(LEDBLUE, HIGH);
    digitalWrite(LEDRED, LOW);
    led1.on();
    led2.off();
  }
  else if (buttonchanal1s == 1)
  {
    digitalWrite(relaychanal1, HIGH);
    Blynk.notify(namedevice + " DOOR OPEN");
    opens = true;
    digitalWrite(LEDBLUE, LOW);
    digitalWrite(LEDRED, HIGH);
    led2.on();
    led1.off();
  }
}
void setup()
{

  Serial.begin(115200);
  pinMode(relaychanal1, OUTPUT);
  digitalWrite(relaychanal1, LOW);
  pinMode(LEDBLUE, OUTPUT);
  digitalWrite(LEDBLUE, HIGH);
  pinMode(LEDRED, OUTPUT);
  digitalWrite(LEDRED, HIGH);
  led1.off();
  led2.off();
  if (strlen(BLYNK_TOKEN) == 0)
  {
    BLYNK_ENABLED = false;
  }
  if (BLYNK_ENABLED)
  {
    Blynk.begin(BLYNK_TOKEN, WIFI_SSID, WIFI_PASS, "blynk-cloud.com", 8442);
    LINE.setToken(LINE_TOKEN);
    LINE.notify(namedevice + " Connect Finish !!");
    LINE.notify("!! Online !!");
    LINE.notify("Start..");
    digitalWrite(LEDBLUE, LOW);
    digitalWrite(LEDRED, LOW);
  }
}
void loop()
{

  if (BLYNK_ENABLED)
  {
    if (Blynk.connected())
    {
      Blynk.run();
    }
  }
  Blynk.virtualWrite(V10, buttonchanal1);
  Blynk.virtualWrite(V12, buttonchanal1);
  Blynk.virtualWrite(V13, buttonchanal1);
  int sensorValue = ESP.getVcc();
  float volts = (float)sensorValue / 1023.00;
  Blynk.virtualWrite(V11, volts);

  if (buttonchanal1 == 0 && closes == true)
  {
    LINE.notify(namedevice + " DOOR CLOSR");
    closes = false;
  }
  if (buttonchanal1 == 1 && opens == true)
  {
    LINE.notify(namedevice + " DOOR OPEN");
    opens = false;
  }
  if (buttonchanal1s == 0 && closes == true)
  {
    LINE.notify(namedevice + " DOOR CLOSR");
    closes = false;
  }
  if (buttonchanal1s == 1 && opens == true)
  {
    LINE.notify(namedevice + " DOOR OPEN");
    opens = false;
  }
}
