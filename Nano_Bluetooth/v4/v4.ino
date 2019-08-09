#include <MCP4922.h>
#include <SPI.h>

MCP4922 DAC(11, 13, 10, 5); // (MOSI,SCK,CS,LDAC) define Connections for UNO_board,

int incomingByte = 0; // for incoming serial data
int number = 0;
void setup()
{
  Serial.begin(9600);
  SPI.begin();
}

void loop()
{
  //DAC.Set(0, 0); //1.0 v
  //delay(500);
  //DAC.Set(820, 820); //1.0 v
  //delay(500);
  //DAC.Set(1630, 1630); //2.0 v
  //delay(500);
  //DAC.Set(2440, 2440); //3.0 v
  //delay(500);
  DAC.Set(3270, 3370); //4.0 v
  //delay(500);
  //DAC.Set(4095,4095); //5.0 v
  //delay(500);

}
