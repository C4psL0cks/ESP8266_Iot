#include <AntoIO.h>

const char *ssid = "6021607";
const char *pass = "17401449";
const char *user = "Luciano_1";
const char *token = "vp127KXKiDme0plYnpXgK3K1U25QA39Hpll0vMfq";
const char *thing = "Nodemcu";

int Read;
int number;

// initialize AntoIO instance
AntoIO anto(user, token, thing);

int value = 0;

void setup() {

  Serial.begin(115200);
  while (!Serial) {}
  delay(10);
  pinMode(2, OUTPUT);

  Serial.println();
  Serial.println();
  Serial.print("Anto library version: ");
  Serial.println(anto.getVersion());

  Serial.print("\nTrying to connect ");
  Serial.print(ssid);
  Serial.println("...");

  anto.begin(ssid, pass, messageReceived);
  Serial.println("\nConnected Anto done");

  //Subscript Channels
  anto.sub("Char");

}
void loop() {
  anto.mqtt.loop();
  if (Serial.available() > 0)
  {
    Read = Serial.read();
    if (Read == 49) {
      number  = 1;
      anto.pub("Char", number);
    }
    if (Read == 48) {
      number = 0;
      anto.pub("Char", number);
    }
  }
  int state = Serial.parseInt(); 
  if (state == 1) 
  {
    digitalWrite(value , HIGH);
    Serial.println("Command completed LED turned ON");
  }
  if (state == 2)
  {
    digitalWrite(value , LOW);
    Serial.println("Command completed LED turned OFF");
  }
}
void messageReceived(String thing, String channel, String payload) {

  Serial.print("Recieved: ");
  Serial.print(thing);
  Serial.print("/");
  Serial.print(channel);
  Serial.print("-> ");
  Serial.println(payload);

  if (channel.equals("Char")) {
    value = payload.toInt();
    if (value == 1) {
      digitalWrite(2, HIGH);
      Serial.println("Command completed LED turned ON");
    }
    else {
      digitalWrite(2, LOW);
      Serial.println("Command completed LED turned OFF");
    }
  }
}
