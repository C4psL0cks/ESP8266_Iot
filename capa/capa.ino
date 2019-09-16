#include <CapacitiveSensor.h>

int csSum[] = {0, 0, 0};


CapacitiveSensor   cs_2_3 = CapacitiveSensor(2, 3);       // 10 megohm resistor between pins 4 & 2, pin 2 is sensor pin, add wire, foil
CapacitiveSensor   cs_4_5 = CapacitiveSensor(4, 5);       // 10 megohm resistor between pins 4 & 2, pin 2 is sensor pin, add wire, foil
CapacitiveSensor   cs_6_7 = CapacitiveSensor(6, 7);       // 10 megohm resistor between pins 4 & 2, pin 2 is sensor pin, add wire, foil

void setup()
{

  cs_2_3.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  cs_4_5.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  cs_6_7.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  Serial.begin(115200);
  while (!Serial) {}

}

void loop()
{
  //  CSread();
  long start = millis();

  long total1 =  cs_2_3.capacitiveSensor(30);
  long total2 =  cs_4_5.capacitiveSensor(30);
  long total3 =  cs_6_7.capacitiveSensor(30);
  //CSread(total1, 0, 0, 0);
  //CSread(total2, 1, 0, 0);
  //CSread(total3, 2, 0, 0);

  //  Serial.print(millis() - start);        // check on performance in milliseconds
  //  Serial.print("\t");                    // tab character for debug window spacing
  //
  Serial.println(total1);                  // print sensor output 1
  //    Serial.println(total2);                  // print sensor output 1
  //    Serial.println(total3);                  // print sensor output 1
  delay(10);                             // arbitrary delay to limit data to serial port
}


void CSread(long cspin, int num, int pass, int thresold) {


  //  Serial.print(": ");
  //    Serial.println(cspin);

  if (cspin > 100) { //b: Arbitrary number
    csSum[num] += cspin;
    //    Serial.println(cspin);
    if (csSum[num] >= 2000) //c: This value is the threshold, a High value means it takes longer to trigger
    {
      Serial.println(num);
      if (csSum[num] > 0) {
        csSum[num] = 0;  //Reset
      }
      switch (num) {
        case 0:
          cs_2_3.reset_CS_AutoCal(); //Stops readings
          break;
        case 1:
          cs_4_5.reset_CS_AutoCal(); //Stops readings
          break;
        case 2:
          cs_6_7.reset_CS_AutoCal(); //Stops readings
          break;
      }
    }
  } else {
    csSum[num] = 0; //Timeout caused by bad readings
  }
}
