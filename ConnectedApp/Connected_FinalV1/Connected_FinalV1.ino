#include <MCP4922.h>
#include <SPI.h>
MCP4922 DAC(11, 13, 10, 5); // (MOSI,SCK,CS,LDAC) define Connections for UNO_board,
#include <SoftwareSerial.h>
SoftwareSerial bluetooth(2, 3);   //TX, RX

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
#define PIN_HIGH 1 // 1
#define PIN_LOW 0  // 0
#define Pin_chanal1 6
#define Pin_chanal2 7

String Text;
char get_Star = ' ';
int Read_command = 0;
int pin_number_digital = 0;
int pin_number_analog = 0;
int pin_state = 0;
int pin_value = 0;
int pin_value_state = 0;
int statuscase;
char send_status_pincheck;
int count = 0;
int pin_number;

void setup() {
  Serial.begin(9600);
  pinMode(Pin_chanal1, OUTPUT);
  digitalWrite(Pin_chanal1, HIGH);
  pinMode(Pin_chanal2, OUTPUT);
  digitalWrite(Pin_chanal2, HIGH);
  delay(1000);
  digitalWrite(Pin_chanal1, LOW);
  digitalWrite(Pin_chanal2, LOW);
  SPI.begin();
  while (!Serial) ;
  bluetooth.begin(9600);
  Serial.println("Connected...");
  DAC.Set(0, 0); //1.0 v
  DAC.Set(2410, 0); //3.0 v
}
void loop() {

  if (Serial.available()) {
    bluetooth.write(Serial.read());
  }
  if (bluetooth.available()) {
    get_Star = bluetooth.read();
  }

  if (get_Star != START_CMD_CHAR) {
    return;
  }
  else {
    Read_command = bluetooth.parseInt();
    if (Read_command == CMD_DIGITALWRITE) {
      Serial.println("CMD_DIGITALWRITE");
      pin_number_digital = bluetooth.parseInt();
      pin_state = bluetooth.parseInt();

      if (pin_state == PIN_LOW) {
        pin_value_state = LOW;
      }
      if (pin_state == PIN_HIGH) {
        pin_value_state = HIGH;
      }
      speed_run(pin_number_digital, pin_value_state);
      return;
    }
    if (Read_command == CMD_READ_ARDUDROID) {
      Serial.println("CMD_READ_ARDUDROID");
      char send_to_android[] = "sensor1";
      String sends;
      sends += String(statuscase) + "," + String(pin_number_digital) + "," + String(pin_value_state);
      bluetooth.print(sends);
      Serial.println(sends);
      //bluetooth.print(String(send_status_pincheck) + "\n");
      return;
    }
  }
}

void speed_run(int pin_number, int pin_value) {

  switch (pin_number) {
    case 0:
      DAC.Set(0, 0); //0.0 v
      statuscase = 0;
      break;
    case 1:
      if (pin_value == 1) {
        DAC.Set(810, 0); //1.0 v
        statuscase = 1;
      }
      else {
        DAC.Set(0, 0); //1.0 v
        statuscase = 0;
      }
      break;
    case 2:
      if (pin_value == 1) {
        DAC.Set(1610, 0); //2.0 v
        statuscase = 2;
      }
      else {
        DAC.Set(0, 0); //1.0 v
        statuscase = 0;
      }
      break;
    case 3:
      if (pin_value == 1) {
        DAC.Set(2410, 0); //3.0 v
        statuscase = 3;
      }
      else {
        DAC.Set(0, 0); //1.0 v
        statuscase = 0;
      }
      break;
    case 4:
      if (pin_value == 1) {
        DAC.Set(3280, 0); //4.0 v
        statuscase = 4;
      }
      else {
        DAC.Set(0, 0); //1.0 v
        statuscase = 0;
      }
      break;
    case 5:
      if (pin_value == 1) {
        DAC.Set(4095, 4095); //5.0 v
        statuscase = 5;
      }
      else {
        DAC.Set(0, 0); //1.0 v
        statuscase = 0;
      }
      break;
    case 6:
      digitalWrite(Pin_chanal1, pin_value);
      send_status_pincheck = pin_value;
      break;
    case 7:
      digitalWrite(Pin_chanal2, pin_value);
      send_status_pincheck = pin_value;
      break;
  }
}
