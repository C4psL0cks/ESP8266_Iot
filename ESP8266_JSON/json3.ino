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
  Serial.println();
  Serial.println();
  Serial.println(out);

  StaticJsonBuffer<200> msgBuffer;   //Memory pool
  JsonObject& msg = msgBuffer.parseObject(out); //Parse message

  String msgName =  msg["name"];
  int msgAge = msg["age"];

  Serial.print("Name = "); Serial.println(msgName);
  Serial.print("Age = "); Serial.println(msgAge);
}

void loop() {
  // put your main code here, to run repeatedly:

}
