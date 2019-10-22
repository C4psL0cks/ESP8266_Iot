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

const int sensorPin0 = A0;
const int sensorPin1 = A1;
const int sensorPin2 = A2;
const int sensorPin3 = A3;
const int sensorPin4 = A4;
const int sensorPin5 = A5;
bool check0 = false;
bool check1 = false;
bool check2 = false;
bool check3 = false;
bool check4 = false;
bool check5 = false;
bool around0 = false;
bool around1 = false;
bool around2 = false;
bool around3 = false;
bool around4 = false;
bool around5 = false;
int value0, values0;
int value1, values1;
int value2, values2;
int value3, values3;
int value4, values4;
int value5, values5;

void setup() {

  Serial.begin(115200);
  pinMode(sensorPin0, INPUT);
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(sensorPin3, INPUT);
  pinMode(sensorPin4, INPUT);
  pinMode(sensorPin5, INPUT);
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

  value0 = analogRead(sensorPin0);
  values0 = map(value0, 0, 1023, 0, 255); //Map value 0-1023 to 0-255 (PWM)

  if (values0 < 200) {
    values0 = 0;
    around0 = false;
  }
  if (values0 > 200) {
    check0 = true;
    if (check0 == true && around0 == false) {
      sends(0);
      check0 = false;
      around0 = true;
    }
  }

  value1 = analogRead(sensorPin1);
  values1 = map(value1, 0, 1023, 0, 255); //Map value 0-1023 to 0-255 (PWM)

  if (values1 < 200) {
    values1 = 0;
    around1 = false;
  }
  if (values1 > 200) {
    check1 = true;
    if (check1 == true && around1 == false) {
      sends(1);
      check1 = false;
      around1 = true;
    }
  }

  value2 = analogRead(sensorPin2);
  values2 = map(value2, 0, 1023, 0, 255); //Map value 0-1023 to 0-255 (PWM)

  if (values2 < 200) {
    values2 = 0;
    around2 = false;
  }
  if (values2 > 200) {
    check2 = true;
    if (check2 == true && around2 == false) {
      sends(2);
      check2 = false;
      around2 = true;
    }
  }

  value3 = analogRead(sensorPin3);
  values3 = map(value3, 0, 1023, 0, 255); //Map value 0-1023 to 0-255 (PWM)

  if (values3 < 200) {
    values3 = 0;
    around3 = false;
  }
  if (values3 > 200) {
    check3 = true;
    if (check3 == true && around3 == false) {
      sends(3);
      check3 = false;
      around3 = true;
    }
  }

  value4 = analogRead(sensorPin4);
  values4 = map(value4, 0, 1023, 0, 255); //Map value 0-1023 to 0-255 (PWM)

  if (values4 < 200) {
    values4 = 0;
    around4 = false;
  }
  if (values4 > 200) {
    check4 = true;
    if (check4 == true && around4 == false) {
      sends(4);
      check4 = false;
      around4 = true;
    }
  }

  value5 = analogRead(sensorPin5);
  values5 = map(value5, 0, 1023, 0, 255); //Map value 0-1023 to 0-255 (PWM)

  if (values5 < 200) {
    values5 = 0;
    around5 = false;
  }
  if (values5 > 200) {
    check5 = true;
    if (check5 == true && around5 == false) {
      sends(5);
      check5 = false;
      around5 = true;
    }
  }
  client.send("topic", "message", "sync");
  delay(500);
  if (client.monitor()) {
    if (RID == "topic" && Rname == "accept") {
      Serial.println("Accept is :" + String(Rcontent) + " ");
    }
  }
}
void sends(int number) {
  Serial.println("Send message:" + String(number) + "OK");
  switch (number) {
    case 0:
      client.send("topic", "message", String(number));
      break;
    case 1:
      client.send("topic", "message", String(number));
      break;
    case 2:
      client.send("topic", "message", String(number));
      break;
    case 3:
      client.send("topic", "message", String(number));
      break;
    case 4:
      client.send("topic", "message", String(number));
      break;
    case 5:
      client.send("topic", "message", String(number));
      break;
  }
}
