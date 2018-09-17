#include <Wire.h>
#include "Adafruit_MCP23017.h"
Adafruit_MCP23017 mcp;

int pin_led = 8; // IC ขา 8
int pin_relay = 9;
int pin_door = 10;
int pin_buzzer = 12;
int doorState = 0;
int ledState = 0;
#define closed 0
#define opened 1

void setup() {
  Serial.begin(115200);
  mcp.begin(); // use default address 0
  mcp.pinMode(pin_led, OUTPUT);
  mcp.pinMode(pin_door, INPUT);
  mcp.pinMode(pin_relay, OUTPUT);
  mcp.pinMode(pin_buzzer, OUTPUT);
  mcp.digitalWrite(pin_buzzer, HIGH);
  mcp.pullUp(pin_door, HIGH);
}


// flip the pin #0 up and down

void loop() {


  //mcp.digitalWrite(pin_led, HIGH);
  //mcp.digitalWrite(pin_relay, LOW);
  //delay(2000);
  //mcp.digitalWrite(pin_led, LOW);
  //mcp.digitalWrite(pin_relay, HIGH);
  //delay(2000);

  //  if ( mcp.digitalRead(pin_door) == LOW && doorState == opened) {
  //    Serial.println("Close");
  //    doorState = closed;
  //    ledState = 1; //on
  //    mcp.digitalWrite(pin_led, ledState);
  //  }
  //  if ( mcp.digitalRead(pin_door) == HIGH && doorState == closed) {
  //
  //    Serial.println("Open");
  //    doorState = opened;
  //    ledState = 0; //off
  //    mcp.digitalWrite(pin_led, ledState);
  //  }
  tone(pin_buzzer, 1000); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(pin_buzzer);     // Stop sound...
  delay(1000);        // ...for 1sec

}
