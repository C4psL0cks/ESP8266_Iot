/*
  MCP4922 test
  most code borrowed from
  http://mrbook.org/blog/2008/11/22/controlling-a-gakken-sx-150-synth-with-arduino/

  connections
  ====================================================

  +5v           > 4922 pin 1
  Ard pin 10    > 4922 pin 3   (SS - slave select)
  Ard pin 13    > 4922 pin 4   (SCK - clock)
  Ard pin 11    > 4922 pin 5   (MOSI - data out)
  Ground        > 4922 pin 8   (LDAC)
  +5v           > 4922 pin 11  (voltage ref DAC B)
  Ground        > 4922 pin 12
  +5v           > 4922 pin 13  (voltage ref DAC A)
  4922 pin 14 DAC A > 1k resistor > synth CV in

*/

// MCP4922 DAC out
#define DATAOUT 11//MOSI
#define DATAIN 12//MISO - not used, but part of builtin SPI
#define SPICLOCK  13//sck
#define SLAVESELECT0 10//ss

int i = 0;

void setup() {
  SPI_setup();
  Serial.begin(9600);
}

void loop() {
  i++;
  //Serial.println("Number: "+String(i));
  //delay(50);
  write_note(1260);
  if (i >= 4096) {
    i = 0;
  }
}

void write_note(int i) {
  write_valueY(i);
  write_valueX(i);
}

// **************************************************
// SPI for DAC

void SPI_setup() {

  byte clr;
  pinMode(DATAOUT, OUTPUT);
  pinMode(SPICLOCK, OUTPUT);
  pinMode(SLAVESELECT0, OUTPUT);
  digitalWrite(SLAVESELECT0, HIGH); //disable device

  SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR1) | (0 << SPR0);
  clr = SPSR;
  clr = SPDR;
  delay(10);
}

// write out through DAC A
void write_valueX(int sample)
{
  // splits int sample in to two bytes
  byte dacSPI0 = 0;
  byte dacSPI1 = 0;
  dacSPI0 = (sample >> 8) & 0x00FF; //byte0 = takes bit 15 - 12
  dacSPI0 |= (1 << 7);
  // A/B: DACa or DACb - Forces 7th bit  of    x to be 1. all other bits left alone.
  dacSPI0 |= 0x10;
  dacSPI1 = sample & 0x00FF; //byte1 = takes bit 11 - 0
  dacSPI0 |= (1 << 5); // set gain of 1
  digitalWrite(SLAVESELECT0, LOW);
  SPDR = dacSPI0;  // Start the transmission
  while (!(SPSR & (1 << SPIF)))   // Wait the end of the transmission
  {
  };

  SPDR = dacSPI1;
  while (!(SPSR & (1 << SPIF)))   // Wait the end of the transmission
  {
  };
  digitalWrite(SLAVESELECT0, HIGH);
  //delay(2);
}

// write out through DAC B
void write_valueY(int sample)
{
  // splits int sample in to two bytes
  byte dacSPI0 = 0;
  byte dacSPI1 = 0;
  dacSPI0 = (sample >> 8) & 0x00FF; //byte0 = takes bit 15 - 12
  dacSPI0 |= 0x10;

  dacSPI1 = sample & 0x00FF; //byte1 = takes bit 11 - 0
  dacSPI0 |= (1 << 5); // set gain of 1

  digitalWrite(SLAVESELECT0, LOW);
  SPDR = dacSPI0;  // Start the transmission
  while (!(SPSR & (1 << SPIF)))   // Wait the end of the transmission
  {
  };

  SPDR = dacSPI1;
  while (!(SPSR & (1 << SPIF)))   // Wait the end of the transmission
  {
  };
  digitalWrite(SLAVESELECT0, HIGH);
}
