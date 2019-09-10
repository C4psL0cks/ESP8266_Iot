#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>
// Config connect WiFi
#define WIFI_SSID "xxxxx"
#define WIFI_PASSWORD "xxxxx"

// Line config
#define LINE_TOKEN "xxxxxx"

#define IR_SW D7

String msg_you_have_got_mail = "%E0%B8%84%E0%B8%B8%E0%B8%93%E0%B8%A1%E0%B8%B5%E0%B8%88%E0%B8%94%E0%B8%AB%E0%B8%A1%E0%B8%B2%E0%B8%A2";
String msg_pieces = "%E0%B8%89%E0%B8%9A%E0%B8%B1%E0%B8%9A";

int mail_counter;
boolean line_send = false;
unsigned long time_ms_buffer;


void IntSwitch(void)
{
  if ((millis() - time_ms_buffer) > 1000)  // wait for 3 sec
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
  Serial.println();

  WiFi.mode(WIFI_STA);
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
   LINE.setToken(LINE_TOKEN);
  Serial.println(LINE.getVersion());
}

void loop()
{
  if (line_send == true)
    LINE.notify("DHT22");
    LINE.notify(message);
    LINE.notifySticker(3, 240);
    LINE.notifySticker("Hello", 1, 2);
    LINE.notifyPicture("https://xxx.jpg");
    LINE.notifyPicture("xxxx", "https://xxx.jpg");
  delay(5);
}
