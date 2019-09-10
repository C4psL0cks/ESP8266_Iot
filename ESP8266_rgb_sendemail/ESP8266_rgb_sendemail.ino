#include <ESP8266WiFi.h>

// Config connect WiFi
#define WIFI_SSID "Project"
#define WIFI_PASSWORD "0947409034"

// Line config
#define LINE_TOKEN "bSneBQbLceBHeUUC7cv7p6ET0f90gcAVCyXNlwQpLvs"

#define IR_SW D7

String msg_you_have_got_mail = "%E0%B8%84%E0%B8%B8%E0%B8%93%E0%B8%A1%E0%B8%B5%E0%B8%88%E0%B8%94%E0%B8%AB%E0%B8%A1%E0%B8%B2%E0%B8%A2";
String msg_pieces = "%E0%B8%89%E0%B8%9A%E0%B8%B1%E0%B8%9A";

int mail_counter;
boolean line_send = false;
unsigned long time_ms_buffer;
int val = 0;


void Line_Notify_Send(void)
{
  WiFiClientSecure client;

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return;
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + msg_you_have_got_mail + mail_counter + msg_pieces).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + msg_you_have_got_mail + mail_counter + msg_pieces;
  // Serial.println(req);
  client.print(req);

  delay(20);
  line_send = false;

  // Serial.println("-------------");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    //Serial.println(line);
  }
  // Serial.println("-------------");
}

void IntSwitch(void)
{
  if ((millis() - time_ms_buffer) > 1000)  // wait for 1 sec
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
}

void loop()
{
  val = digitalRead(IR_SW);
  Serial.println(val);
  if (line_send == true)
  {
    Line_Notify_Send();
  }
  delay(5);
}
