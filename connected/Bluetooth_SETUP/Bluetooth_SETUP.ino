//HM-10 Setup
#include <SoftwareSerial.h>
SoftwareSerial bluetooth(2, 3); //tx,rx

void setup() {
  Serial.begin(9600);  
  bluetooth.begin(9600);   
}

void loop() {
  if (Serial.available()) {
    bluetooth.write(Serial.read());
  }
  if (bluetooth.available()) {
    Serial.write(bluetooth.read());
  }
}

//Command              Description
//AT                    Check if the command terminal work normally
//AT+RESET              Software reboot
//AT+VERSION            Get firmware, bluetooth, HCI and LMP version
//AT+HELP               List all the commands
//AT+NAME               Get/Set local device name
//AT+PIN                Get/Set pin code for pairing
//AT+PASS               Get/Set pin code for pairing
//AT+BAUD               Get/Set baud rate
//AT+LADDR              Get local bluetooth address
//AT+ADDR               Get local bluetooth address
//AT+DEFAULT            Restore factory default
//AT+RENEW              Restore factory default
//AT+STATE              Get current state
//AT+PWRM               Get/Set power on mode(low power)
//AT+POWE               Get/Set RF transmit power
//AT+SLEEP              Sleep mode
//AT+ROLE               Get/Set current role
//AT+PARI               Get/Set UART parity bit
//AT+STOP               Get/Set UART stop bit
//AT+START              System start working
//AT+IMME               System wait for command when power on
//AT+IBEA               Switch iBeacon mode
//AT+IBE0               Set iBeacon UUID 0
//AT+IBE1               Set iBeacon UUID 1
//AT+IBE2               Set iBeacon UUID 2
//AT+IBE3               Set iBeacon UUID 3
//AT+MARJ               Set iBeacon MARJ
//AT+MINO               Set iBeacon MINO
//AT+MEA                Set iBeacon MEA
//AT+NOTI               Notify connection event
//AT+UUID               Get/Set system SERVER_UUID
//AT+CHAR               Get/Set system CHAR_UUID
//AT+INQ                Search slave model
//AT+SHOW               Show the searched slave model
//AT+CONN               Connect the index slave model
