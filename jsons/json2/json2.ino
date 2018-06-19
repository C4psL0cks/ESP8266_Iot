#include <ArduinoJson.h>
void setup() {
  Serial.begin(9600);
  StaticJsonBuffer<200> JSONBuffer;   //Memory pool
  JsonObject& root = JSONBuffer.createObject(); //Parse message
  String name = "suport";
  int age = 56;

  root["name"] = name;
  root["age"] = age;

  String out = "";
  root.printTo(out);
  Serial.println(out);
}

void loop() {
  // put your main code here, to run repeatedly:

}
