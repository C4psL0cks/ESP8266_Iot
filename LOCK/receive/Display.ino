#include <TimerOne.h> // เรียก scan 7Segment อัตโนมัติ

byte data[3]; // digit ทั้ง 3 ตัวเก็บอะไรไว้บ้าง

#define CK1 A5 // ขาที่ต่อกับ digit
#define CK2 A4
#define CK3 A3

void out7seg(byte data) { //code 7segment
  for (int i=0;i<7;i++)
    digitalWrite(13 - i, (0x01<<i)&data);
/*
  digitalWrite(13, 0b00000001&data);
  digitalWrite(12, 0b00000010&data);
  digitalWrite(11, 0b00000100&data);
  digitalWrite(10, 0b00001000&data);
  digitalWrite(9,  0b00010000&data);
  digitalWrite(8,  0b00100000&data);
  digitalWrite(7,  0b01000000&data);
  */
}

int nDigi = 0; //nDigi เป็นตัวแปรเก็บค่า digit ที่ติดอยู่

void displayText(char *font) { 
  memcpy(data, font, 3); //copy font into data
}

void DisplaySetup() { // Pinmode 
  for (int i=7;i<=13;i++)
    pinMode(i, OUTPUT);

  pinMode(CK1, OUTPUT);
  pinMode(CK2, OUTPUT);
  pinMode(CK3, OUTPUT);
  digitalWrite(CK1, 1);
  digitalWrite(CK2, 1);
  digitalWrite(CK3, 1);

  Timer1.initialize(1000); // ตัวเลขที่ให้เรียกอะไรสักอย่างทุกๆ 1000 micro sec.
  Timer1.attachInterrupt([]() { // ชุดที่อยู่ในปีกกา ให้เรียกมารับทุกๆ 1000 micro sec.
    digitalWrite(CK1 - nDigi, 1); //42-47 คือ code scan 7Segment 
    nDigi = (nDigi + 1) > 2 ? 0 : nDigi + 1;
    // int n = count / (int)pow(10, nDigi) % 10;
    out7seg(data[2-nDigi]);
    digitalWrite(CK1 - nDigi, 0);
  });
  
  memset(data, 0, 3); // set ให้สิ่งที่อยู่ใน data ทั้ง 3 ตัวให้เป็น 0
}

