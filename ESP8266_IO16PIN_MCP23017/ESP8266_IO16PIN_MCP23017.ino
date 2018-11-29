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
  mcp.pinMode(pin_relay, OUTPUT);
}

// flip the pin #0 up and down
void loop() {
  mcp.digitalWrite(pin_led, HIGH);
  mcp.digitalWrite(pin_relay, LOW);
  delay(2000);
  mcp.digitalWrite(pin_led, LOW);
  mcp.digitalWrite(pin_relay, HIGH);
  delay(2000);
}
