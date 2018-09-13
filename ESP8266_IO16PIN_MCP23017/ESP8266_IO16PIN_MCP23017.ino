#include <Wire.h>
#include "Adafruit_MCP23017.h"
Adafruit_MCP23017 mcp;

int pin_led = 8; // IC ขา 8
int pin_relay = 9;

void setup() {
  mcp.begin(); // use default address 0
  mcp.pinMode(pin_led, OUTPUT);
  mcp.pinMode(pin_relay, OUTPUT);
  // mcp.pullUp(pin_sw, HIGH);
}


// flip the pin #0 up and down

void loop() {

  //int val = mcp.digitalRead(pin_sw);
  mcp.digitalWrite(pin_led, HIGH);
  mcp.digitalWrite(pin_relay, LOW);
  delay(2000);
  mcp.digitalWrite(pin_led, LOW);
  mcp.digitalWrite(pin_relay, HIGH);
  delay(2000);

}
