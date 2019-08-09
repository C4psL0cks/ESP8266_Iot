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
#define PIN_HIGH 3
#define PIN_LOW 2
#define Pin_chanal0 5
#define Pin_chanal1 6
#define Pin_chanal2 7
#define Pin_chanal3 8
#define Pin_chanal4 9

String Text;
char get_Star = ' ';
int Read_command = 0;
int pin_number_digital = 0;
int pin_number_analog = 0;
int pin_state = 0;
int pin_value = 0;
int pin_value_state = 0;

void setup() {
  Serial.begin(9600);
  pinMode(Pin_chanal2, OUTPUT);
  digitalWrite(Pin_chanal2, HIGH);
  pinMode(Pin_chanal3, OUTPUT);
  digitalWrite(Pin_chanal3, HIGH);
  SPI.begin();
  while (!Serial) ;
  bluetooth.begin(9600);
  Serial.println("Speed-airflow-bluetooth");

}
void loop() {

  if (Serial.available()) {  // Read serial
    bluetooth.write(Serial.read());   // bluetooth = Serial.read() data form blue send to
    //Serial.println("ddd");
  }

  if (bluetooth.available()) {  // get blue to show in serialmonitor
    //Serial.write(bluetooth.read()); // print blue form serial
    //parse incoming command start flag output = *10|1|3# HIGH
    get_Star = bluetooth.read();
  }

  if (get_Star != START_CMD_CHAR) {
    return;
  }
  else {
    // parse incoming command type 10|
    Read_command = bluetooth.parseInt(); // read the command
    //Serial.println("Read_command:" + String(Read_command));

    // 1) GET digitalWrite DATA FROM ARDUDROID *10|
    if (Read_command == CMD_DIGITALWRITE) {
      Serial.println("CMD_DIGITALWRITE");
      // digital
      //*10|1|3# HIGH
      //*10|1|2# LOW

      // parse incoming pin# and value |1|
      pin_number_digital = bluetooth.parseInt(); // read the pin
      //Serial.println("Pin_number_digital:" + String(pin_number_digital));

      // parse incoming pin# and value |3|#
      pin_state = bluetooth.parseInt(); // read the pin
      //Serial.println("Pin_state:" + String(pin_state));

      if (pin_state == PIN_LOW) {
        //Serial.println("Pin_state LOW:" + String(pin_state));
        pin_value_state = LOW;
      }
      if (pin_state == PIN_HIGH) {
        //Serial.println("Pin_state HIGH:" + String(pin_state));
        pin_value_state = HIGH;
      }
      // send pin_number and pin_value to function
      speed_run(pin_number_digital, pin_value_state);
      return;
    }

    // 2) GET analogWrite DATA FROM ARDUDROID *11|
    if (Read_command == CMD_ANALOGWRITE) {
      Serial.println("CMD_ANALOGWRITE");

      // analog
      //*11|2|93#
      //*11|2|92#

      // parse incoming pin# and value |2|
      pin_number_analog = bluetooth.parseInt(); // read the pin
      //Serial.println("Pin_number_analog:" + String(pin_number_analog));

      // parse incoming pin# and value |93|#
      pin_value = bluetooth.parseInt();  // read the value
      //Serial.println("Pin_value:" + String(pin_value));

      //analogWrite( pin_num, pin_value );
      return;  // Done. return to loop();
    }

    // 3) GET TEXT COMMAND FROM ARDUDROID
    if (Read_command == CMD_TEXT) {
      Serial.println("CMD_TEXT");

      // text
      //*12|ggggggg#

      Text = ""; //clears variable for new input
      while (bluetooth.available())  {
        char text_bluetooth = bluetooth.read();  //gets one byte from serial buffer
        //Serial.println(text_bluetooth);
        delay(5);
        if (text_bluetooth != DIV_CMD_CHAR) { // div
          Text += text_bluetooth;
          delay(5);
        }
        else {
          if (text_bluetooth == END_CMD_CHAR) { // end
            break;
          }
        }
      }
      Serial.print("Text: " + Text);
    }

    // 4) SEND DATA TO ARDUDROID
    if (Read_command == CMD_READ_ARDUDROID) {
      Serial.println("CMD_READ_ARDUDROID");
      //char send_to_android[] = "Place your text here." ;
      //bluetooth.print(send_to_android);   // Example: Sending text
      return;  // Done. return to loop();
    }
  }
}

// 2) select the requested pin# for DigitalWrite action
void speed_run(int pin_number, int pin_value) {

  switch (pin_number) {
    case 0:
      DAC.Set(0, 0); //0.0 v
      Serial.println("pin_number:" + String(pin_number));
      Serial.println("pin_value:" + String(pin_value));
      break;
    case 1:
      if (pin_value == 1) {
        DAC.Set(820, 820); //1.0 v
      }
      else {
        DAC.Set(0, 0); //1.0 v
      }
      break;
    case 2:
      if (pin_value == 1) {
        DAC.Set(1630, 1630); //2.0 v
      }
      else {
        DAC.Set(0, 0); //1.0 v
      }
      break;
    case 3:
      if (pin_value == 1) {
        DAC.Set(2440, 2440); //3.0 v
      }
      else {
        DAC.Set(0, 0); //1.0 v
      }
      break;
    case 4:
      if (pin_value == 1) {
        DAC.Set(3270, 3270); //4.0 v
      }
      else {
        DAC.Set(0, 0); //1.0 v
      }
      break;
    case 5:
      if (pin_value == 1) {
        DAC.Set(4095, 4095); //5.0 v
      }
      else {
        DAC.Set(0, 0); //1.0 v
      }
      break;
    case 6:
      digitalWrite(Pin_chanal2, pin_value);
      break;
    case 7:
      digitalWrite(Pin_chanal3, pin_value);
      break;
  }
}
