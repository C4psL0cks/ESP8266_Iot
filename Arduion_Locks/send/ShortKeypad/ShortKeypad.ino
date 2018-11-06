#include<SoftwareSerial.h>
SoftwareSerial mySerial(2,3);
//                   0     1     2     3     4     5     6     7     8     9     *     #
byte keycode[] = { 0xE5, 0x73, 0x75, 0x76, 0xB3, 0xB5, 0xB6, 0xD3, 0xD5, 0xD6, 0xE3, 0xE6 };

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  // Col (INPUT)
  pinMode(5, INPUT);
  pinMode(4, INPUT);
  pinMode(13, INPUT);

  // Row (OUTPUT)
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
}

char ScanKey() {
  char c = 0;
  for (int i=0;i<12;i++) {
    digitalWrite(9, keycode[i]&0b00010000);
    digitalWrite(8, keycode[i]&0b00100000);
    digitalWrite(7, keycode[i]&0b01000000);
    digitalWrite(6, keycode[i]&0b10000000);

    byte key = (keycode[i]&0xF0)|(digitalRead(5)<<2)|(digitalRead(4)<<1)|digitalRead(13);
    if (key == keycode[i]) {
     c = (i < 10 ? i + '0' : i == 10 ? '*' : i == 11 ? '#' : 0);
    }
  }

  return c;
}

void loop() {
  char key = ScanKey();
  if (key != 0) {
    while(ScanKey() == key) delay(10);

    if(key == '1')
      mySerial.print('1');
    else if (key == '2')
      mySerial.print('4');
    else if (key == '3')
      mySerial.print('7');
    else if (key == '4')
      mySerial.print('2');
    else if (key == '5')
      mySerial.print('5');
    else if (key == '6')
      mySerial.print('8');
    else if (key == '7')
      mySerial.print('3');
    else if (key == '8')
      mySerial.print('6');
    else if (key == '9')
      mySerial.print('9');
    else if (key == '*')
      mySerial.print('*');
    else if (key == '0')
      mySerial.print('#');
    else if (key == '#')
      mySerial.print('0');
  }
}
