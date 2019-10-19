#include <MCP3008.h>
#define CS_PIN D8
#define CLOCK_PIN D5
#define MOSI_PIN D7
#define MISO_PIN D6
MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);

#include "Adafruit_MCP23017.h"
Adafruit_MCP23017 mcp;

ADC_MODE(ADC_VCC);

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
int val, vals;
int sensorValue, volts;

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
  sensorValue = ESP.getVcc();
  volts = (float)sensorValue / 1023.00;
  Serial.println("Volts in ESP8266 : " + String(volts));

  int val0 = adc.readADC(0); // read Channel 0 from MCP3008 ADC (pin 0)
  Serial.println(val0);
  int val1 = adc.readADC(1); // read Channel 0 from MCP3008 ADC (pin 2)
  Serial.println(val1);
  int val2 = adc.readADC(2); // read Channel 0 from MCP3008 ADC (pin 3)
  Serial.println(val2);
  int val3 = adc.readADC(3); // read Channel 0 from MCP3008 ADC (pin 4)
  Serial.println(val3);
  int val4 = adc.readADC(4); // read Channel 0 from MCP3008 ADC (pin 5)
  Serial.println(val4);
  int val5 = adc.readADC(5); // read Channel 0 from MCP3008 ADC (pin 6)
  Serial.println(val5);
  int val6 = adc.readADC(6); // read Channel 0 from MCP3008 ADC (pin 7)
  Serial.println(val6);
  int val7 = adc.readADC(7); // read Channel 0 from MCP3008 ADC (pin 8)
  Serial.println(val7);

  //  mcp.digitalWrite(pin_1, HIGH);
  //  mcp.digitalWrite(pin_2, HIGH);
  //  mcp.digitalWrite(pin_3, HIGH);
  //  mcp.digitalWrite(pin_4, HIGH);
  //  mcp.digitalWrite(pin_5, HIGH);
  //  mcp.digitalWrite(pin_6, HIGH);
  //  mcp.digitalWrite(pin_7, HIGH);
  //  mcp.digitalWrite(pin_8, HIGH);
  //  mcp.digitalWrite(pin_28, HIGH);
  //  mcp.digitalWrite(pin_27, HIGH);
  //  mcp.digitalWrite(pin_26, HIGH);
  //  mcp.digitalWrite(pin_25, HIGH);
  //  mcp.digitalWrite(pin_24, HIGH);
  //  mcp.digitalWrite(pin_23, HIGH);
  //  mcp.digitalWrite(pin_22, HIGH);
  //  mcp.digitalWrite(pin_21, HIGH);
  //  delay(1000);
  //  mcp.digitalWrite(pin_1, LOW);
  //  mcp.digitalWrite(pin_2, LOW);
  //  mcp.digitalWrite(pin_3, LOW);
  //  mcp.digitalWrite(pin_4, LOW);
  //  mcp.digitalWrite(pin_5, LOW);
  //  mcp.digitalWrite(pin_6, LOW);
  //  mcp.digitalWrite(pin_7, LOW);
  //  mcp.digitalWrite(pin_8, LOW);
  //  mcp.digitalWrite(pin_28, LOW);
  //  mcp.digitalWrite(pin_27, LOW);
  //  mcp.digitalWrite(pin_26, LOW);
  //  mcp.digitalWrite(pin_25, LOW);
  //  mcp.digitalWrite(pin_24, LOW);
  //  mcp.digitalWrite(pin_23, LOW);
  //  mcp.digitalWrite(pin_22, LOW);
  //  mcp.digitalWrite(pin_21, LOW);
  //  delay(1000);
  delay(100);
}
