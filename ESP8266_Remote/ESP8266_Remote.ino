
/*
   IRremoteESP8266: IRServer - demonstrates sending IR codes controlled from a webserver
   An IR LED must be connected to ESP8266 GPIO pin 15.
   Version 0.1 June, 2015

   Modified by Alex Willis for Instructables
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const char* ssid = "6021607";
const char* password = "17401449";
char Name[20] = "";
MDNSResponder mdns;

ESP8266WebServer server(80);
IRsend irsend(4);

void handleRoot() {
  server.send(200, "text/html", "<html>"
              "<head><title>Add your title here</title></head>"
              "<body>"
              "<h1>Add a header here</h1>"
              "<p><a href=\"ir?Power=551489775\">Power 0x40040100BCBD</a></p>"
              "<p><a href=\"ir?Midiaplayer=551502015\">Midiaplayer 0x40040190D544</a></p>"
              "<p><a href=\"ir?Menu=551534655\">Menu 0x400401004A4B</a></p>"
              "<p><a href=\"ir?Tv=551498445\">Tv 0x400401400C4D</a></p>"
              "<p><a href=\"ir?Av=551493855\">Av 0x40040100A0A1</a></p>"
              "<p><a href=\"ir?Up=551512725\">Up 0x400401005253</a></p>"
              "<p><a href=\"ir?Down=551545365\">Down 0x40040100D2D3</a></p>"
              "<p><a href=\"ir?Left=551492325\">Left 0x400401007273</a></p>"
              "<p><a href=\"ir?Right=551524965\">Right 0x40040100F2F3</a></p>"
              "<p><a href=\"ir?Ok=551508645\">Ok 0x400401009293</a></p>"
              "<p><a href=\"ir?Back=551535165\">Back 0x400401002B2A</a></p>"
              "<p><a href=\"ir?Exit=551535258\">Exit 0x40040100CBCA</a></p>"
              "<p><a href=\"ir?VolUp=551535456\">VolUp 0x400401000405</a></p>"
              "<p><a href=\"ir?VolDown=551535457\">VolDown 0x400401008485</a></p>"
              "<p><a href=\"ir?Aspect=551535189\">Aspect 0x400401207B5A</a></p>"
              "<p><a href=\"ir?Mute=551535174\">Mute 0x400401004C4D</a></p>"
              "<p><a href=\"ir?Chup=551535125\">Chup 0x400401002C2D</a></p>"
              "<p><a href=\"ir?Chdown=551535158\">Chdown 0x40040100ACAD</a></p>"
              "</body>"
              "</html>");

}
void handleIr() {
  for (uint8_t i = 0; i < server.args(); i++) {
    Serial.println(server.argName(i));
    //server.argName(i).toCharArray(Name, 20);
    //Serial.println(Name);
    if (server.argName(i) == "Power") {
      Serial.println("Power");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x40040100BCBD, 48);
    }
    else if (server.argName(i) == "Midiaplayer") {
      Serial.println("Midiaplayer");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x40040190D544 , 48);
    }
    else if (server.argName(i) == "Menu") {
      Serial.println("Menu");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x400401004A4B , 48);
    }
    else if (server.argName(i) == "TV") {
      Serial.println("TV");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x400401400C4D , 48);
    }
    else if (server.argName(i) == "AV") {
      Serial.println("AV");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x40040100A0A1 , 48);
    }
    else if (server.argName(i) == "Up") {
      Serial.println("Up");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x400401005253 , 48);
    }
    else if (server.argName(i) == "Down") {
      Serial.println("Down");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x40040100D2D3 , 48);
    }
    else if (server.argName(i) == "Left") {
      Serial.println("Left");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x400401007273 , 48);
    }
    else if (server.argName(i) == "Right") {
      Serial.println("Right");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x40040100F2F3 , 48);
    }
    else if (server.argName(i) == "Ok") {
      Serial.println("Ok");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x400401009293 , 48);
    }
    else if (server.argName(i) == "Back") {
      Serial.println("Back");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x400401002B2A , 48);
    }
    else if (server.argName(i) == "Exit") {
      Serial.println("Exit");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x40040100CBCA , 48);
    }
    else if (server.argName(i) == "VolUp") {
      Serial.println("VolUp");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x400401000405 , 48);
    }
    else if (server.argName(i) == "VolDown") {
      Serial.println("VolDown");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x400401008485 , 48);
    }
    else if (server.argName(i) == "Aspect") {
      Serial.println("Aspect");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x400401207B5A , 48);
    }
    else if (server.argName(i) == "Mute") {
      Serial.println("Mute");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x400401004C4D , 48);
    }
    else if (server.argName(i) == "Chup") {
      Serial.println("Chup");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x400401002C2D , 48);
    }
    else if (server.argName(i) == "Chdown") {
      Serial.println("Chdown");
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(code);
      irsend.sendNEC(0x40040100ACAD , 48);
    }
  }

  server.on("/", handleRoot);
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

void setup(void) {
  irsend.begin();

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/ir", handleIr);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
