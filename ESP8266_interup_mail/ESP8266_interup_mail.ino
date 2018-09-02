#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>
// Config connect WiFi
#define WIFI_SSID "6021607"
#define WIFI_PASSWORD "17401449"

// Line config
#define LINE_TOKEN "xxxx"
#define IR_SW D7

int mail_counter;
boolean line_send = false;
unsigned long time_ms_buffer;
int val = 0;

void IntSwitch(void) {
  if ((millis() - time_ms_buffer) > 1000) {
    mail_counter++;
    line_send = true;
  }
  time_ms_buffer = millis();
}
void setup() {
  Serial.begin(115200);

  pinMode(IR_SW, INPUT);
  attachInterrupt(digitalPinToInterrupt(IR_SW), IntSwitch, FALLING);
  Serial.println();
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  LINE.setToken(LINE_TOKEN);
  Serial.println(LINE.getVersion());
}

void loop()
{
  val = digitalRead(IR_SW);
  if (line_send == true) {
    LINE.notifySticker("Your got mail" + String(mail_counter), 1, 2);
  }
  delay(5);
}
