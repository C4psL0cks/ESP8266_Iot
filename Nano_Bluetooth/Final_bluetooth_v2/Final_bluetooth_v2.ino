#include <SoftwareSerial.h>
SoftwareSerial bluetooth(2, 3);   //RX, TX
String Text;
#define START_CMD_CHAR '*'
#define DIV_CMD_CHAR '|'
#define END_CMD_CHAR '#'
#define CMD_DIGITALWRITE 10
#define CMD_ANALOGWRITE 11
#define CMD_TEXT 12
#define CMD_READ_ARDUDROID 13
#define MAX_COMMAND 20  // max command number code. used for error checking.
#define MIN_COMMAND 10  // minimum command number code. used for error checking. 
#define IN_STRING_LENGHT 40
#define MAX_ANALOGWRITE 255
#define PIN_HIGH 3
#define PIN_LOW 2

void setup() {
  Serial.begin(9600);
  while (!Serial) ;
  bluetooth.begin(9600);
  Serial.println("Hello");

}
void loop() {

  int Read_command = 0;
  int pin_num = 0;
  int pin_value = 0;

  char get_Star = ' ';  //read serial

  if (Serial.available()) {  // อ่านค่าจาก serial มา
    bluetooth.write(Serial.read());   // bluetooth = Serial.read() ที่อ่านมาได้จาก blue ส่งมา
  }

  if (bluetooth.available()) {  // รับค่าจาก blue มาแล้วมาปริ้นออก serial หน้าจอ
    //Serial.write(bluetooth.read()); // ปริ้น ออหหน้าจอโดยเอาค่าจาก blue มาปริ้น
    //parse incoming command start flag
    get_Star = bluetooth.read();
  }

  if (get_Star != START_CMD_CHAR) {
    return; // if no command start flag, return to loop().
  }
  else {
    // parse incoming command type
    Read_command = bluetooth.parseInt(); // read the command
    Serial.println("ard_command:" + String(Read_command));

    // 1) GET digitalWrite DATA FROM ARDUDROID
    if (Read_command == CMD_DIGITALWRITE) {
      Serial.println("CMD_DIGITALWRITE");
      //      if (pin_value == PIN_LOW) {
      //        pin_value = LOW;
      //      }
      //      else if (pin_value == PIN_HIGH) {
      //        pin_value = HIGH;
      //      }
      //      else {
      //        return; // error in pin value. return.
      //      }

      return;  // return from start of loop()
    }

    // 2) GET analogWrite DATA FROM ARDUDROID
    if (Read_command == CMD_ANALOGWRITE) {
      Serial.println("CMD_ANALOGWRITE");
      //analogWrite( pin_num, pin_value );
      return;  // Done. return to loop();
    }

    // 3) GET TEXT COMMAND FROM ARDUDROID
    if (Read_command == CMD_TEXT) {
      Text = ""; //clears variable for new input
      while (Serial.available())  {
        char c = Serial.read();  //gets one byte from serial buffer
        Serial.println(c);
        delay(5);
        if (c == END_CMD_CHAR) { // if we the complete string has been read
          // add your code here
          break;
        }
        else {
          if (c !=  DIV_CMD_CHAR) {
            Text += c;
            delay(5);
          }
        }
      }
    }

  }

  //  // 1) GET TEXT COMMAND FROM ARDUDROID
  //  if (ard_command == CMD_TEXT) {
  //    inText = ""; //clears variable for new input
  //    while (bluetooth.available())  {
  //      char text_bluetooth = bluetooth.read();  //gets one byte from serial buffer
  //      //Serial.println(text_bluetooth);
  //      delay(5);
  //      if (text_bluetooth != DIV_CMD_CHAR) { // div
  //        inText += text_bluetooth;
  //        delay(5);
  //      }
  //      else {
  //        if (text_bluetooth == END_CMD_CHAR) { // end
  //          break;
  //        }
  //      }
  //    }
  //    Serial.print("inText: " + inText);
  //  }
  //
  //  // 2) GET digitalWrite DATA FROM ARDUDROID
  //  if (ard_command == CMD_DIGITALWRITE) {
  //    Serial.println("CMD_DIGITALWRITE");
  //    // parse incoming pin# and value
  //
  //    pin_num = bluetooth.parseInt(); // read the pin
  //    //Serial.println("pin_num:" + String(pin_num));
  //    if (pin_value == PIN_LOW) {
  //      pin_value = LOW;
  //    }
  //    else if (pin_value == PIN_HIGH) {
  //      pin_value = HIGH;
  //    }
  //    else {
  //      return; // error in pin value. return.
  //    }
  //    speed_run( pin_num,  pin_value);  // Uncomment this function if you wish to use
  //    return;  // return from start of loop()
  //  }
  //
  //  // 3) GET analogWrite DATA FROM ARDUDROID
  //  if (ard_command == CMD_ANALOGWRITE) {
  //    Serial.println("CMD_ANALOGWRITE");
  //    // parse incoming pin# and value
  //    pin_value = bluetooth.parseInt();  // read the value
  //    //Serial.println("pin_value:" + String(pin_value));
  //    analogWrite(pin_num, pin_value);
  //    // add your code here
  //    return;  // Done. return to loop();
  //  }
  //
  //  // 4) SEND DATA TO ARDUDROID
  //  if (ard_command == CMD_READ_ARDUDROID) {
  //    // char send_to_android[] = "Place your text here." ;
  //    // Serial.println(send_to_android);   // Example: Sending text
  //    Serial.print(" Analog 0 = ");
  //    Serial.println(analogRead(A0));  // Example: Read and send Analog pin value to Arduino
  //    return;  // Done. return to loop();
  //  }

}

// 2a) select the requested pin# for DigitalWrite action
void speed_run(int pin_num, int pin_value)
{
  switch (pin_num) {
    case 13:
      pinMode(13, OUTPUT);
      digitalWrite(13, pin_value);
      // add your code here
      break;
    case 12:
      pinMode(12, OUTPUT);
      digitalWrite(12, pin_value);
      // add your code here
      break;
    case 11:
      pinMode(11, OUTPUT);
      digitalWrite(11, pin_value);
      // add your code here
      break;
    case 10:
      pinMode(10, OUTPUT);
      digitalWrite(10, pin_value);
      // add your code here
      break;
    case 9:
      pinMode(9, OUTPUT);
      digitalWrite(9, pin_value);
      // add your code here
      break;
    case 8:
      pinMode(8, OUTPUT);
      digitalWrite(8, pin_value);
      // add your code here
      break;
    case 7:
      pinMode(7, OUTPUT);
      digitalWrite(7, pin_value);
      // add your code here
      break;
    case 6:
      pinMode(6, OUTPUT);
      digitalWrite(6, pin_value);
      // add your code here
      break;
    case 5:
      pinMode(5, OUTPUT);
      digitalWrite(5, pin_value);
      // add your code here
      break;
    case 4:
      pinMode(4, OUTPUT);
      digitalWrite(4, pin_value);
      // add your code here
      break;
    case 3:
      pinMode(3, OUTPUT);
      digitalWrite(3, pin_value);
      // add your code here
      break;
    case 2:
      pinMode(2, OUTPUT);
      Serial.println("2");
      digitalWrite(2, pin_value);
      // add your code here
      break;
      // default:
      // if nothing else matches, do the default
      // default is optional
  }
}
