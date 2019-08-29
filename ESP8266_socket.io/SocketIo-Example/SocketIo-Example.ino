#include <SocketIOClient.h>

#define LedPin 2
#define ButtonPin 0

const char* ssid = "6021607";
const char* password = "17401449";
String host = "192.168.0.10";
int port = 3000;
bool clicked = false;

SocketIOClient socket;

void setupNetwork() {

  WiFi.begin(ssid, password);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  if (i == 21) {
    while (1) delay(500);
  }

}
void click() {
  clicked = true;
}
void light(String state) {
  Serial.println("[light] " + state);
  if (state == "\"state\":true") {
    Serial.println("[light] ON");
    digitalWrite(LedPin, HIGH);
  }
  else {
    Serial.println("[light] off");
    digitalWrite(LedPin, LOW);
  }
}
//
// This code runs only once
//
void setup() {

  // set up our pins
  pinMode(LedPin, OUTPUT);
  pinMode(ButtonPin, INPUT);

  digitalWrite(LedPin, LOW);

  Serial.begin(115200);

  setupNetwork();

  attachInterrupt(digitalPinToInterrupt(ButtonPin), click, FALLING);

  socket.on("light", light);

  socket.connect(host, port);
}

void clickCheck() {
  if (clicked) {
    Serial.println("[click]");
    socket.emit("toggle", "{\"state\":true}");
    clicked = false;
  }
}

//
// This code runs over and over again
//
void loop() {
  socket.monitor();
  clickCheck();
}
