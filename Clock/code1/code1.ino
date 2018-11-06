#include <DS1307RTC.h>
#include <Time.h>
#include <Wire.h>

const int hr1pin=5;
const int hr2pin=6;
const int mn1pin=7;
const int mn2pin=12;
const int sc1pin=8;
const int sc2pin=9;
const int rst=13;

int hr1=0;
int hr2=0;
int mn1=0;
int mn2=0;
int sc1=0;
int sc2=0;
 void setup()
 {
   pinMode(hr1pin,OUTPUT);
   pinMode(hr2pin,OUTPUT);
   pinMode(mn1pin,OUTPUT);
   pinMode(mn2pin,OUTPUT);
   pinMode(sc1pin,OUTPUT);
   pinMode(sc2pin,OUTPUT);
   Serial.begin(9600);
   Serial.print("Read initial time : ");
   tmElements_t tm;

  if (RTC.read(tm)) {
    hr1= tm.Hour/10;
    hr2= tm.Hour%10;
    mn1= tm.Minute/10;
    mn2= tm.Minute%10;
    sc1= tm.Second/10;
    sc2= tm.Second%10;
    Serial.print(hr1);
    Serial.print(hr2);
    Serial.print(":");
    Serial.print(mn1);
    Serial.print(mn2);
    Serial.print(":");
    Serial.print(sc1);
    Serial.println(sc2);
  }
  Serial.println("Enter cnt");
  delay(100);
  initialSet();
 }
 
 void loop()
 {
   tmElements_t tm;

  if (RTC.read(tm)) {
    hr1= tm.Hour/10;
    hr2= tm.Hour%10;
    mn1= tm.Minute/10;
    mn2= tm.Minute%10;
    sc1= tm.Second/10;
    sc2= tm.Second%10;
   Serial.print(sc2);
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.");
    } else {
      Serial.println("DS1307 read error!");
    }
    delay(9000);
  }
  delay(100);
 }
 
 void initialSet()
 {
   for(int a=0;a<hr1;a++)
   {
     digitalWrite(hr1pin,HIGH);
     delayMicroseconds(1);
     digitalWrite(hr1pin,LOW);
     delayMicroseconds(1);
   }
   for(int a=0;a<hr2;a++)
   {
     digitalWrite(hr2pin,HIGH);
     delayMicroseconds(1);
     digitalWrite(hr2pin,LOW);
     delayMicroseconds(1);
   }
   for(int a=0;a<mn1;a++)
   {
     digitalWrite(mn1pin,HIGH);
     delayMicroseconds(1);
     digitalWrite(mn1pin,LOW);
     delayMicroseconds(1);
   }
   for(int a=0;a<mn2;a++)
   {
     digitalWrite(mn2pin,HIGH);
     delayMicroseconds(1);
     digitalWrite(mn2pin,LOW);
     delayMicroseconds(1);
   }
   for(int a=0;a<sc1;a++)
   {
     digitalWrite(sc1pin,HIGH);
     delayMicroseconds(1);
     digitalWrite(sc1pin,LOW);
     delayMicroseconds(1);
   }
   for(int a=0;a<sc2;a++)
   {
     digitalWrite(sc2pin,HIGH);
     delayMicroseconds(1);
     digitalWrite(sc2pin,LOW);
     delayMicroseconds(1);
   }
 }
