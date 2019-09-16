#include "SocketIOClient.h"
SocketIOClient client;
byte mac[] = { 0xAA, 0x00, 0xBE, 0xEF, 0xFE, 0xEE };
char hostip[] = "192.168.1.34";
int port = 8000;
int log1, log2, log3;

#include <CapacitiveSensor.h>
int csSum[] = {0, 0, 0};
CapacitiveSensor  cs_2_3 = CapacitiveSensor(2, 3);
CapacitiveSensor  cs_4_5 = CapacitiveSensor(4, 5);
CapacitiveSensor  cs_6_7 = CapacitiveSensor(6, 7);

void setup() {

  cs_2_3.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs_4_5.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs_6_7.set_CS_AutocaL_Millis(0xFFFFFFFF);

  Serial.begin(9600);
  Ethernet.begin(mac);
  Serial.print("This IP address: ");
  IPAddress myIPAddress = Ethernet.localIP();
  Serial.print(myIPAddress);
  Serial.println("connecting...");

  while (!client.connect(hostip, port)) {
    Serial.println("connection failed");
  }
  if (client.connected()) {
    Serial.println("Connect");
  }
}

void loop() {
  //client.monitor();
  long total1 =  cs_2_3.capacitiveSensor(30);
  long total2 =  cs_4_5.capacitiveSensor(30);
  long total3 =  cs_6_7.capacitiveSensor(30);
  CSread(total1, 0, 130, 1500);
  CSread(total2, 1, 200, 2000);
  CSread(total3, 2, 100, 1000);

  //  Serial.print(millis() - start);        // check on performance in milliseconds
  //  Serial.print("\t");                    // tab character for debug window spacing
  //
  Serial.println(total1);                  // print sensor output 1
  //    Serial.println(total2);                  // print sensor output 1
  //      Serial.println(total3);                  // print sensor output 1
  //client.sendJSON("toggle1", String(total2));
  //client.sendJSON("toggle2", "{\"Log2\": \"GGEZ2\"}");
  client.sendJSON("toggle", String(total3));

  delay(50);

}
void CSread(long cspin, int num, int pass, int thresold) {

  //  Serial.print(": ");
  //    Serial.println(cspin);
  if (cspin > pass) { //b: Arbitrary number
    csSum[num] += cspin;
    //    Serial.println(cspin);
    if (csSum[num] >= thresold) //c: This value is the threshold, a High value means it takes longer to trigger
    {
      //Serial.println(num);
      if (num == 0) {
        client.sendJSON("toggle1", "{\"Log\": \"GGEZ1\"}");
      }
      if (num == 1) {
        client.sendJSON("toggle1", "{\"Log\": \"GGEZ2\"}");
      }
      if (num == 2) {
        client.sendJSON("toggle1", "{\"Log\": \"GGEZ3\"}");
      }
      if (csSum[num] > 0) {
        csSum[num] = 0;  //Reset
      }
      switch (num) {
        case 0:
          cs_2_3.reset_CS_AutoCal(); //Stops readings
          break;
        case 1:
          cs_4_5.reset_CS_AutoCal(); //Stops readings
          break;
        case 2:
          cs_6_7.reset_CS_AutoCal(); //Stops readings
          break;
      }
    }
  } else {
    csSum[num] = 0; //Timeout caused by bad readings
  }
}
