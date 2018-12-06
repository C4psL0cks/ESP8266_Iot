#include <Wire.h>
#include "Adafruit_MCP23017.h"
Adafruit_MCP23017 mcp;

// ซ้ายบนลงล่าง
int pin_1 = 8;
int pin_2 = 9;
int pin_3 = 10;
int pin_4 = 11;
int pin_5 = 12;
int pin_6 = 13;
int pin_7 = 14;
int pin_8 = 15;

//ขวาบนลงล่าง
int pin_28 = 7;
int pin_27 = 6;
int pin_26 = 5;
int pin_25 = 4;
int pin_24 = 3;
int pin_23 = 2;
int pin_22 = 1;
int pin_21 = 0;


void setup() {
  Serial.begin(115200);
  mcp.begin(); 
  mcp.pinMode(pin_1, OUTPUT);
  mcp.pinMode(pin_2, OUTPUT);
  mcp.pinMode(pin_3, OUTPUT);
  mcp.pinMode(pin_4, OUTPUT);
  mcp.pinMode(pin_5, OUTPUT);
  mcp.pinMode(pin_6, OUTPUT);
  mcp.pinMode(pin_7, OUTPUT);
  mcp.pinMode(pin_8, OUTPUT);
  mcp.pinMode(pin_28, OUTPUT);
  mcp.pinMode(pin_27, OUTPUT);
  mcp.pinMode(pin_26, OUTPUT);
  mcp.pinMode(pin_25, OUTPUT);
  mcp.pinMode(pin_24, OUTPUT);
  mcp.pinMode(pin_23, OUTPUT);
  mcp.pinMode(pin_22, OUTPUT);
  mcp.pinMode(pin_21, OUTPUT);
}

void loop() {
  mcp.digitalWrite(pin_1, HIGH);
  mcp.digitalWrite(pin_2, LOW);
  delay(2000);
  mcp.digitalWrite(pin_1, LOW);
  mcp.digitalWrite(pin_2, HIGH);
  delay(2000);
}
