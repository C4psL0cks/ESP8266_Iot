#include <SoftwareSerial.h>
#include <MCP4922.h>
#include <SPI.h>
SoftwareSerial bluetooth(2, 3);
MCP4922 DAC(11, 13, 10, 5);

int incomingcase;
int pin_number;
#define Pin_chanal1 6
#define Pin_chanal2 7

void setup() {
  Serial.begin(9600);
  pinMode(Pin_chanal1, OUTPUT);
  digitalWrite(Pin_chanal1, HIGH);
  pinMode(Pin_chanal2, OUTPUT);
  digitalWrite(Pin_chanal2, HIGH);
  delay(1000);
  digitalWrite(Pin_chanal1, LOW);
  digitalWrite(Pin_chanal2, LOW);
  SPI.begin();
  while (!Serial);
  bluetooth.begin(9600);
  Serial.println("\nConnected device tunebyaot ...version 1.0.0");
  DAC.Set(0, 0); //1.0 v
  DAC.Set(2410, 0); //3.0 v
  //DAC.Set(3370, 0); //3.0 v
}

void loop() {

  if (Serial.available()) {
    bluetooth.write(Serial.read());
  }
  if (bluetooth.available()) {
    incomingcase = bluetooth.read();
    //Serial.println(incomingcase);
    switch (incomingcase) {
      case 0:
        DAC.Set(0, 0); //0.0 v
        Serial.println("CMD_0");
        break;
      case 11:
        //DAC.Set(810, 0); //1.0 v
        DAC.Set(450, 0); //1.0 v
        Serial.println("CMD_11");
        break;
      case 21:
        //DAC.Set(1610, 0); //2.0 v
        DAC.Set(1480, 0); //2.0 v
        Serial.println("CMD_21");
        break;
      case 31:
        //DAC.Set(2410, 0); //3.0 v
        DAC.Set(2410, 0); //3.0 v
        Serial.println("CMD_31");
        break;
      case 41:
        //DAC.Set(3280, 0); //4.0 v
        DAC.Set(3370, 0); //4.0 v
        Serial.println("CMD_41");
        break;
      case 51:
        //DAC.Set(4095, 0); //5.0 v
        DAC.Set(4095, 4095); //5.0 v
        Serial.println("CMD_51");
        break;
      case 60:
        Serial.println("CMD_60");
        digitalWrite(Pin_chanal1, LOW);
        break;
      case 61:
        Serial.println("CMD_61");
        digitalWrite(Pin_chanal1, HIGH);
        break;
      case 70:
        Serial.println("CMD_70");
        digitalWrite(Pin_chanal2, LOW);
        break;
      case 71:
        Serial.println("CMD_71");
        digitalWrite(Pin_chanal2, HIGH);
        break;
    }
  }
}
