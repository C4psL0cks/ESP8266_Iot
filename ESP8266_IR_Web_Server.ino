/*
   IRremoteESP8266: IRServer - demonstrates sending IR codes controlled from a webserver
   An IR LED must be connected to ESP8266 GPIO pin 15.
   Version 0.1 June, 2015

   Modified by Alex Willis for Instructables
*/

#include <FirebaseArduino.h>
#define FIREBASE_HOST "xx"
#define FIREBASE_AUTH "xxx"
#define WIFI_SSID "xxx"
#define WIFI_PASSWORD "xxx"

const int LED_PIN1 = D6;
const int LED_PIN2 = D3;
const int LED_PIN3 = D4;
const int FAN_PIN = D7;

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

MDNSResponder mdns;
ESP8266WebServer server(80);
IRsend irsend(4);

void setup(void) {

  Serial.begin(115200);
  irsend.begin();
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  Serial.println(WiFi.localIP());
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.setInt("Control/LED1", 0);  // led 0 - 1
  Firebase.setInt("Control/LED2", 0);  // led 0 - 1
  Firebase.setInt("Control/LED3", 0);  // led 0 - 1
  Firebase.setInt("Control/FAN", 0); // fan 0 - 0

  server.on("/", handleRoot); // รอรับปุ่มกดจากหน้าเว็บ
  server.on("/ir", handleIr); // ปุ่มถูกกด ส่งไปเช้คว่าเลขอะไร

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  digitalWrite(LED_PIN1, Firebase.getInt("Control/LED1"));
  digitalWrite(LED_PIN2, Firebase.getInt("Control/LED2"));
  digitalWrite(LED_PIN3, Firebase.getInt("Control/LED3"));
  digitalWrite(FAN_PIN, Firebase.getInt("Control/FAN"));
  delay(100);

}
void handleRoot() {
  server.send(200, "text/html", "<html>"
              "<head><title>Add your title here</title></head>"
              "<body>"
              "<h1>Add a header here</h1>"
              "<p><a href=\"ir?code=1\">Power 0x40040100BCBD</a></p>"
              "<p><a href=\"ir?code=2\">Midiaplayer 0x40040190D544</a></p>"
              "<p><a href=\"ir?code=3\">Menu 0x400401004A4B</a></p>"
              "<p><a href=\"ir?code=4\">Tv 0x400401400C4D</a></p>"
              "<p><a href=\"ir?code=5\">Av 0x40040100A0A1</a></p>"
              "<p><a href=\"ir?code=6\">Up 0x400401005253</a></p>"
              "<p><a href=\"ir?code=7\">Down 0x40040100D2D3</a></p>"
              "<p><a href=\"ir?code=8\">Left 0x400401007273</a></p>"
              "<p><a href=\"ir?code=9\">Right 0x40040100F2F3</a></p>"
              "<p><a href=\"ir?code=10\">Ok 0x400401009293</a></p>"
              "<p><a href=\"ir?code=11\">Back 0x400401002B2A</a></p>"
              "<p><a href=\"ir?code=12\">Exit 0x40040100CBCA</a></p>"
              "<p><a href=\"ir?code=13\">VolUp 0x400401000405</a></p>"
              "<p><a href=\"ir?code=14\">VolDown 0x400401008485</a></p>"
              "<p><a href=\"ir?code=15\">Aspect 0x400401207B5A</a></p>"
              "<p><a href=\"ir?code=16\">Mute 0x400401004C4D</a></p>"
              "<p><a href=\"ir?code=17\">Chup 0x400401002C2D</a></p>"
              "<p><a href=\"ir?code=18\">Chdown 0x40040100ACAD</a></p>"
              "</body>"
              "</html>");
}
void handleIr() {
  for (uint8_t i = 0; i < server.args(); i++) {
    //Serial.println(server.argName(i));
    if (server.argName(i) == "code")
    {
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      if (code == 1) {
        irsend.sendNEC(0x40040100BCBD, 48);
      }
      if (code == 2) {
        irsend.sendNEC(0x40040190D544, 48);
      }
      if (code == 3) {
        irsend.sendNEC(0x400401004A4B, 48);
      }
      if (code == 4) {
        irsend.sendNEC(0x400401400C4D, 48);
      }
      if (code == 5) {
        irsend.sendNEC(0x40040100A0A1, 48);
      }
      if (code == 6) {
        irsend.sendNEC(0x400401005253, 48);
      }
      if (code == 7) {
        irsend.sendNEC(0x40040100D2D3, 48);
      }
      if (code == 8) {
        irsend.sendNEC(0x400401007273, 48);
      }
      if (code == 9) {
        irsend.sendNEC(0x40040100F2F3, 48);
      }
      if (code == 10) {
        irsend.sendNEC(0x400401009293, 48);
      }
      if (code == 11) {
        irsend.sendNEC(0x400401002B2A, 48);
      }
      if (code == 12) {
        irsend.sendNEC(0x40040100CBCA, 48);
      }
      if (code == 13) {
        irsend.sendNEC(0x400401000405, 48);
      }
      if (code == 14) {
        irsend.sendNEC(0x400401008485, 48);
      }
      if (code == 15) {
        irsend.sendNEC(0x400401207B5A, 48);
      }
      if (code == 16) {
        irsend.sendNEC(0x400401004C4D, 48);
      }
      if (code == 17) {
        irsend.sendNEC(0x400401002C2D, 48);
      }
      if (code == 18) {
        irsend.sendNEC(0x400401008485, 48);
      }
    }
  }
  handleRoot();
}
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
