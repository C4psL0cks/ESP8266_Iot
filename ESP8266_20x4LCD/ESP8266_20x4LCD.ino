#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 20, 4);

void setup() {
  Serial.begin(9600);
  lcd.begin();
}
void loop() {
  lcd.setCursor(0, 2);
  lcd.print("Nodemcu test LED");
}
