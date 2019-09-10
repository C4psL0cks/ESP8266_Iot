/////// DHT 22 /////////
#include "DHT.h"
#define DHTPIN 13     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors
DHT dht(DHTPIN, DHTTYPE);
int timeSinceLastRead = 0;

/////// DHT 22 /////////

/////// Sero /////////
#include <Servo.h>
Servo myservo1, myservo2, myservo3, myservo4;
/////// Sero /////////

////// LCD ////////
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
////// LCD ////////

//// time //////
#include <DS3231.h>
DS3231  rtc(SDA, SCL);
//// time //////

int count = 0;
int a = 0;

void setup()
{

  /////// DHT 22 /////////
  Serial.begin(9600);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  //while (!Serial) { }

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");

  /////// DHT 22 /////////

  //// time //////
  rtc.begin();
  //// time //////

  lcd.begin();
  pinMode(6, INPUT);
  pinMode(myservo1.attach(2), OUTPUT);
  myservo1.write(45);
  pinMode(7, INPUT);
  pinMode(myservo2.attach(3), OUTPUT);
  myservo2.write(45);
  pinMode(8, INPUT);
  pinMode(myservo3.attach(4), OUTPUT);
  myservo3.write(43);
  pinMode(9, INPUT);
  pinMode(myservo4.attach(5), OUTPUT);
  myservo4.write(30);

}
void loop()
{

  // Report every 2 seconds.
  if (timeSinceLastRead > 2000) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    //// time //////
    lcd.setCursor(0, 2);
    //lcd.print("Time:  ");
    lcd.print(rtc.getTimeStr());

    lcd.setCursor(10, 2);
    //lcd.print("Date: ");
    lcd.print(rtc.getDateStr());
    Serial.print(rtc.getDOWStr());
    Serial.print(" ");

    // Send date
    Serial.print(rtc.getDateStr());
    Serial.print(" -- ");
    // Send time
    Serial.println(rtc.getTimeStr());

    //delay (1000);
    //// time //////

    /////// DHT 22 /////////
    lcd.setCursor(5, 0);
    lcd.print("Welcome To");

    lcd.setCursor(3, 3);
    lcd.print(h);

    lcd.setCursor(9, 3);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("Pharmacy  Automation");

    lcd.setCursor(12, 3);
    lcd.print(t);

    lcd.setCursor(18, 3);
    lcd.print("C");

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");
    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100;

  /////// DHT 22 /////////

  int sw = digitalRead(6);
  if (sw == HIGH) {
    lcd.begin();
    lcd.setCursor(2, 1);
    lcd.print("EMERGENCY 1");
    myservo1.write(20);
    delay(400);
    myservo1.write(45);
    delay(400);
    lcd.setCursor(6, 2);
    lcd.print ("Successful!!");
    lcd.begin();
    lcd.setCursor(0, 1);

  }

  int sw1  = digitalRead(7);
  if (sw1 == HIGH) {
    lcd.begin();
    lcd.setCursor(4, 1);
    lcd.print("EMERGENCY 2");
    myservo2.write(10);
    delay(400);
    myservo2.write(45);
    delay(400);
    lcd.setCursor(6, 2);
    lcd.print ("Successful!!");
    lcd.begin();
    lcd.setCursor(0, 1);

  }

  int sw2 = digitalRead(8);
  if (sw2 == HIGH) {
    lcd.begin();
    lcd.setCursor(4, 1);
    lcd.print("EMERGENCY 3");
    myservo3.write(0);
    delay(300);
    myservo3.write(43);
    delay(400);

    lcd.setCursor(6, 2);
    lcd.print ("Successful!!");
    lcd.begin();
    lcd.setCursor(0, 1);

  }

  int sw3 = digitalRead(9);
  if (sw3 == HIGH) {
    lcd.begin();
    lcd.setCursor(4, 1);
    lcd.print("EMERGENCY 4");
    myservo4.write(0);
    delay(200);
    myservo4.write(30);
    delay(200);
    lcd.setCursor(6, 2);
    lcd.print ("Successful!!");
    lcd.begin();
    lcd.setCursor(0, 1);

  }

  int sw4 = digitalRead(10);
  if (sw4 == HIGH) {
    lcd.begin();
    lcd.setCursor(6, 1);
    lcd.print("Morning");
    delay(200);
    myservo1.write(20);
    delay(200);
    myservo1.write(45);
    delay(200);
    myservo2.write(10);
    delay(200);
    myservo2.write(45);
    delay(200);
    lcd.setCursor(6, 2);
    lcd.print ("Successful!!");
    lcd.begin();
    lcd.setCursor(0, 1);
  }

  int sw5 = digitalRead(11);
  if (sw5 == HIGH) {
    lcd.begin();
    lcd.setCursor(6, 1);
    lcd.print("Afternoon");
    delay(200);
    myservo3.write(0);
    delay(200);
    myservo3.write(43);
    delay(200);
    myservo4.write(0);
    delay(200);
    myservo4.write(30);
    delay(200);
    lcd.setCursor(6, 2);
    lcd.print ("Successful!!");
    lcd.begin();
    lcd.setCursor(0, 1);

  }

  int sw6 = digitalRead(12);
  if (sw6 == HIGH) {
    lcd.begin();
    lcd.setCursor(6, 1);
    lcd.print("Evening");
    delay(200);
    myservo1.write(20);
    delay(200);
    myservo1.write(45);
    delay(200);
    myservo4.write(0);
    delay(200);
    myservo4.write(30);
    delay(200);
    lcd.setCursor(6, 2);
    lcd.print ("Successful!!");
    lcd.begin();
    lcd.setCursor(0, 1);
  }
}


