#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // Rx, Tx

#define SERIAL_TYPE mySerial

#define CORRECT_PASSWORD "345"

// -------| Display |-------
//                      0     1     2     3     4     5     6     7     8     9 NULL
const byte font[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0, 0x70, };
byte texLock[3] = { 0x38, 0x3F, 0x39 }; // LOC
byte textErr[3] = { 0x40, 0x79, 0x50 }; // -Er 
byte textOK[3]  = { 0x40, 0x3F, 0x70 }; // -O|-
byte textDLo[3] = { 0x00, 0x3F, 0x70 }; //  O|-
byte textHidePass[4][3] = {
  { 0, 0, 0 },
  { 0, 0, 0x70 },
  { 0, 0x70, 0x70 },
  { 0x70, 0x70, 0x70 }
};
byte textOff[3] = { 0, 0, 0 };
// --------//---------

enum {
  MOTER_STOP,
  MOTER_FORWARD,
  MOTER_BACKWARD,
} MOTER_MOVE_t;

#define LOCK MOTER_FORWARD
#define UNLOCK MOTER_BACKWARD

String inPassword = "";
int wrongCount = 0;

void setup() {
  DisplaySetup();
  MotorSetup();

  MotorSet(LOCK);
  
  SERIAL_TYPE.begin(9600);
}

void loop() {
  if (SERIAL_TYPE.available() > 0) {
    char c = SERIAL_TYPE.read();
    if (c >= '0' && c <= '9') {
      if (inPassword.length() < 3) {
        inPassword += c;
      }
    } else if (c == '*') {
      // On OK
      if (inPassword == CORRECT_PASSWORD) {
        displayText(textOK);
        SERIAL_TYPE.write('O');
        MotorSet(UNLOCK);
        while(1) {
          if (SERIAL_TYPE.available() > 0) {
            if (SERIAL_TYPE.read() == '*') break;
            delay(10);
          }
        }
        displayText(textDLo);
        MotorSet(LOCK);
        wrongCount = 0;
      } else {
        if (wrongCount >= 3) {
          displayText(texLock);
          delay(10 * 1000);
          displayText(textHidePass[0]);
          wrongCount = 0;
        } else {
          displayText(textErr);
          delay(5 * 1000);
          displayText(textHidePass[0]);
          wrongCount++;
        }
      }
      inPassword = "";
    } else if (c == '#') {
      // On CLS
      inPassword = "";
    }
    if (c != '*') {
      displayText(textHidePass[inPassword.length()]);
    }
  }
}
