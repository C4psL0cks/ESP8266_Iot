/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  This sketch shows how to write values to Virtual Pins

  NOTE:
  BlynkTimer provides SimpleTimer functionality:
    http://playground.arduino.cc/Code/SimpleTimer

  App project setup:
    Value Display widget attached to Virtual Pin V5
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h> //Time
#include <WidgetRTC.h> //Time
#include <EEPROM.h>//EEPROM

#include <PZEM004Tv30.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "authtokenblynk";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "name_wifi";
char pass[] = "password";

byte count, count_table = 0;
int v21, v22, Minute, Hour, pin_v24;
float cost_old = 0;
float cost_total = 0;
float cost_today = 0;
long last_time;


PZEM004Tv30 pzem(12, 14);  //D6/Tx pzem ,D5/Rx pzem

BlynkTimer timer;
WidgetRTC rtc;


BLYNK_WRITE(V25) { // Input time ดึงค่าเวลา
  TimeInputParam t(param);
  Hour = t.getStartHour();
  Minute = t.getStartMinute();
}


BLYNK_WRITE(V21)
{
  v21 = param.asInt(); // Get value v21 Selector
}

BLYNK_WRITE(V22)
{
  v22 = param.asInt(); // Get value v22 Date Input
}

BLYNK_WRITE(V23) {         //man reset kWh
  int pin_v23 = param.asInt();
  if (v21 == 2 && pin_v23 == 1) {
    pzem.resetEnergy();
    cost_old = 0;
    EEPROM.put(0, cost_old); // put data EEPROM
    EEPROM.commit();
    Blynk.virtualWrite(V20, "add", count_table, "Man reset kWh", 0); //Add data to table
    EEPROM_count();
  }
}



BLYNK_WRITE(V24) { //reset table
  pin_v24 = param.asInt();

  if (pin_v24 == 1) {
    last_time = millis();
  }

}

BLYNK_CONNECTED() {
  rtc.begin(); // Synchronize time on connection
  Blynk.syncVirtual(V21, V22, V25); //update V21 select,V22 date ,V25 input time

  Blynk.virtualWrite(V20, "add", count_table, "Connected ", 0);
  EEPROM_count();
}

void read_pzem()
{

  float voltage = pzem.voltage();
  if ( !isnan(voltage) ) {
    Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
    Blynk.virtualWrite(V10, voltage);
  } else {
    Serial.println("Error reading voltage");
  }

  float current = pzem.current();
  if ( !isnan(current) ) {
    Serial.print("Current: "); Serial.print(current); Serial.println("A");
    Blynk.virtualWrite(V11, current);
  } else {
    Serial.println("Error reading current");
  }

  float power = pzem.power();
  if ( !isnan(power) ) {
    Serial.print("Power: "); Serial.print(power); Serial.println("W");
    Blynk.virtualWrite(V12, power);
  } else {
    Serial.println("Error reading power");
  }

  float energy = pzem.energy();
  if ( !isnan(energy) ) {
    Serial.print("Energy: "); Serial.print(energy, 3); Serial.println("kWh");
    Blynk.virtualWrite(V13, energy, 3);

    float rate_1 = 0;
    float rate_2 = 0;
    float rate_3 = 0;
    //int x = (int)energy%1;
    if (energy >= 0 && energy <= 150) {   //หน่วยที่ 0-150
      rate_1 = energy * 3.2484;
    }
    if (energy > 150) {
      rate_1 = 150 * 3.2484;
    }

    if (energy >= 151 && energy <= 400) { //หน่วยที่ 151-400
      rate_2 = energy * 4.2218;
    }
    if (energy > 400) { //เกิน 400
      rate_2 = 250 * 4.2218;
      rate_3 = (energy - 400) * 4.4217;
    }
    float ft = energy * (-0.116);
    float total_vat = (rate_1 + rate_2 + rate_3 + ft + 38.22) * 0.07;
    float total = (rate_1 + rate_2 + rate_3 + ft + 38.22) + total_vat;
    cost_total = total;
    Blynk.virtualWrite(V16, total, 1);

  } else {
    Serial.println("Error reading energy");
  }

  float frequency = pzem.frequency();
  if ( !isnan(frequency) ) {
    Serial.print("Frequency: "); Serial.print(frequency, 1); Serial.println("Hz");
    Blynk.virtualWrite(V14, frequency, 1);
  } else {
    Serial.println("Error reading frequency");
  }

  float pf = pzem.pf();
  if ( !isnan(pf) ) {
    Serial.print("PF: "); Serial.println(pf);
    Blynk.virtualWrite(V15, pf);
  } else {
    Serial.println("Error reading power factor");
  }
  Serial.println();

}

void Check_time()
{
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details
  String currentTime = String(hour()) + ":" + minute();// + ":" + second(); //read time
  //String currentDate = String(day()) + "/" + month() + "/" + year();
  String currentDate = String(day()) + "/" + month(); // read date

  if (count == 0 && hour() == Hour && minute() == Minute) { //time compare
    cost_today = cost_total - cost_old;
    Blynk.virtualWrite(V20, "add", count_table, currentDate + " - " + currentTime + " cost : " + cost_today + " Baht", 0); //add data to table
    EEPROM_count();
    cost_old = cost_total; // set cost_total to cost_old
    EEPROM.put(0, cost_old); // put data EEPROM
    EEPROM.commit();
    if (day() == v22 && v21 == 1) { //auto reset kWh
      delay(500);
      pzem.resetEnergy();
      cost_old = 0;
      Blynk.virtualWrite(V20, "add", count_table, currentDate + " - " + currentTime + "Auto reset kWh", 0); //add data to table
      EEPROM_count();
    }
    count = 1;
  } else if (minute() != Minute && count > 0) {
    count = 0;
  }

}

void setup()
{
  // Debug console
  Serial.begin(9600);
  EEPROM.begin(256);
  EEPROM.get(0, cost_old); //Get data EEPROM
  EEPROM.get(20, count_table); //Get data EEPROM
  //Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  Blynk.begin(auth, ssid, pass, "192.168.0.123", 8080);
  //Blynk.begin(auth, ssid, pass, IPAddress(192.168.0.123), 8080);
  setSyncInterval(10 * 60); // Sync interval in seconds (10 minutes)
  // Setup a function to be called every second
  timer.setInterval(2000L, read_pzem);
  timer.setInterval(20000L, Check_time);
}

void EEPROM_count()
{
  count_table++;
  EEPROM.put(20, count_table); // put data EEPROM
  EEPROM.commit();
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer

  if (pin_v24 == 1 && millis() - last_time > 5000) { //กดค้าง 5 Sec เพื่อ reset table
    last_time = millis();
    Blynk.virtualWrite(V20, "clr"); //Clear table
    count_table = 0;
    EEPROM.put(20, count_table); // put data EEPROM
    EEPROM.commit();
    Serial.println("Reset");
  }
}
