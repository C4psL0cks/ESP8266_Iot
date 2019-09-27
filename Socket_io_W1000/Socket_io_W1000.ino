#include "SocketIOClient.h"
#include <ArduinoJson.h>
SocketIOClient client;
byte mac[] = { 0xAA, 0x00, 0xBE, 0xEF, 0xFE, 0xEE };
char hostip[] = "192.168.0.4";
int port = 8000;
char JSONmessageBuffer[300];
int value, value1 = 3;
const int sensorPin = A0; //pin A0 to read analog input
unsigned long startPress = 0;
bool check = true;
extern String topic;
extern String key;
extern String values;

void setup() {
  Serial.begin(115200);
  while (!Serial) continue;
  Ethernet.begin(mac);
  Serial.print("This IP address: ");
  IPAddress myIPAddress = Ethernet.localIP();
  Serial.print(myIPAddress);
  Serial.println("connecting...");
//  while (!client.connect(hostip, port)) {
//    Serial.println("connection failed");
//  }
//  if (client.connected()) {
//    Serial.println("Client connected");
//  }
//  Serial.println("Hello....");
}

void loop() {

  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  //
  //  value = analogRead(sensorPin);
  //  //Serial.println(value);
  //  value = map(value, 0, 1023, 0, 255); //Map value 0-1023 to 0-255 (PWM)
  //  //Serial.println(value);
  //
  //  startPress = millis();
  //  while (analogRead(A0) == LOW) {
  //    unsigned long duration = millis() - startPress;
  //    //Serial.println("duration:" + String(duration));
  //    if (duration > 100 && check == true) {
  //      JSONencoder["message"] = value1;
  //      JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  //      Serial.println(JSONmessageBuffer);
  //      client.sendJSON("topic", JSONmessageBuffer);
  //      check = false;
  //    }
  //  }
  //  check = true;

  JSONencoder["message"] = value1;
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);
  client.sendJSON("topic", JSONmessageBuffer);
  delay(1000);


  //  // recevice
  //  if (client.monitor()) {
  //    // Ex. Sending side(package from server)
  //    // socket.emit("EVENT_NAME", { "ATTRIBUTE_OF_OBJ" : SOMETHING });
  //    if (topic == "topic") {
  //      if (key == "Log") {
  //        Serial.print("Wait ACK :" + values);
  //      }
  //    }
  //  }

}
