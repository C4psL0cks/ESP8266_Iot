#include "SocketIOClient.h"
#include "Ethernet.h"
#include "SPI.h"
SocketIOClient client;

byte Mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
char Hostname[] = "192.168.0.2";
int Port = 3484;

extern String RID;
extern String Rname;
extern String Rcontent;

const int sensorPin = A5;
bool check = false;
bool around = false;
int value, values;

void setup() {

  Serial.begin(115200);
  while (!Serial) continue;
  Ethernet.begin(Mac);
  if (!Ethernet.begin(Mac)) {
    Serial.println(F("Failed to configure Ethernet"));
    return;
  }
  if (!client.connect(Hostname, Port))
    Serial.println(F("Not connected."));
  if (client.connected()) {
    client.send("connection", "message", "Connected !!!!");
  }
  else {
    Serial.println("Connection Error");
    while (1);
  }
  delay(500);
}

void loop() {

  value = analogRead(sensorPin);
  values = map(value, 0, 1023, 0, 255); //Map value 0-1023 to 0-255 (PWM)

  if (values < 200) {
    values = 0;
    around = false;
  }
  if (values > 200) {
    check = true;
    if (check == true && around == false) {
      sends(1);
      check = false;
      around = true;
    }
  }
  client.send("topic", "message", "sync");
  delay(500);
  if (client.monitor()) {
    if (RID == "topic" && Rname == "accept") {
      Serial.print("Accept is :" + String(Rcontent));
    }
  }
}
void sends(int number) {
  Serial.println("OK");
  client.send("topic", "message", String(number));
}
