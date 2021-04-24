#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "IjTF2MXQdWiqQ9-WPMQZmM3GXYAvo5mW";   //  ตัวนี้คือ Tokens ของเราใน blynk
char ssid[] = "PH";                                // ชื่อ wifi ต้องเป็นตัวเลขหรือภาษา อังกฤษ เท่านั้น
char pass[] = "0801279491";                          // รหัส wifi

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SensorPin A0          //pH meter Analog output to Arduino Analog Input A0
unsigned long int avgValue;   //Store the average value of the sensor feedback
float b;
float phValue;
int buf[10], temp;
int relay = D4;

WidgetLCD lcd1(V0);

void setup() {

  Serial.begin(9600);
  lcd.begin();
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  //  lcd.init();
  //  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Hello to");
  lcd.setCursor(2, 1);
  lcd.print("PH control. ");
  //   lcd.setCursor(0,2);
  //  lcd.print("Arduino LCM IIC 2004");
  //   lcd.setCursor(2,3);
  //  lcd.print("Power By Ec-yuan!");
  delay(2000);
  lcd.clear();
  Blynk.begin(auth, ssid, pass);
}

void PH() {

  for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor for smooth the value
  {
    buf[i] = analogRead(SensorPin);
    delay(10);
  }
  for (int i = 0; i < 9; i++) //sort the analog from small to large
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)
    avgValue += buf[i];
  phValue = (float)avgValue * 5.0 / 1024 / 6;
  phValue = 3.5 * phValue;
  //convert the millivolt into pH value
  Serial.print(analogRead(SensorPin));
  Serial.print("    pH:");
  Serial.print(phValue, 2);
  Serial.println(" ");

  lcd.setCursor(0, 0);
  lcd.print("* PH control *");
  lcd.setCursor(2, 1);

  lcd.print("  PH : ");

  lcd.print(phValue, 2);

  Blynk.virtualWrite(V1, phValue);

  lcd1.print(0, 0, "* PH control *");
  lcd1.print(0, 1, "  pH : ");
  lcd1.print(8, 1, phValue);
}
void chack() {
  if (phValue <= 10)    // ตั้ง PH ว่าต้องการให้น้อยกว่าหรือเท่ากับ 7 ปั้มทำงาน  เราสามารถตังเองได้เลยตามต้องการ
  {
    digitalWrite(relay, LOW);
    Blynk.virtualWrite(V2, 0);
  }
  else
  {
    digitalWrite(relay, HIGH);
    Blynk.virtualWrite(V2, 1);
  }
}

void loop() {
  Blynk.run();
  PH();
  chack();
}
