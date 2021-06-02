#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>

#define WIFI_SSID "dlink-ABD0"
#define WIFI_PASS "yyafr68490"

#define LINE_TOKEN "xxxx"
#define IR_SW D7

int val = 0;
int mail_counter;
unsigned long time_ms_buffer;
boolean line_send = false;

void IntSwitch(void)
{
  if ((millis() - time_ms_buffer) > 1000)
  {
    mail_counter++;
    line_send = true;
  }
  time_ms_buffer = millis();
}
void setup()
{

  Serial.begin(115200);
  pinMode(IR_SW, INPUT);
  attachInterrupt(digitalPinToInterrupt(IR_SW), IntSwitch, FALLING);
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n", SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());

  LINE.setToken(LINE_TOKEN);
  Serial.println(LINE.getVersion());
}
void loop()
{
  val = digitalRead(IR_SW);
  if (line_send == true)
  {
    LINE.notifySticker("Your got mail" + String(mail_counter), 1, 2);
  }
  delay(5);
}
