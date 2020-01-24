//KEYPAD
#include <Keypad.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','M'},
  {'4','5','6','C'},
  {'7','8','9','B'},
  {'P','0','N','E'}
};
byte rowPins[ROWS] = {25, 24, 23, 22}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {29, 28, 27, 26}; //connect to the column pinouts of the keypad

const int relayCount = 15;
char relayKey[relayCount] = {'1','2','3','M','4','5','6','C','7','8','9','B','P','0','N'};

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//Serial3 COM.
String incomingData = ""; // for incoming Serial3 data
String outgoingData = ""; // for outgoingData Serial3 data

//DS18B20
/*
#include <OneWire.h> 
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
*/
int TempUpdateCount = 0;

//Buzzer
#define BuzzerPin A0

//Relay
int relayPIN[relayCount];

//OTHER
byte PHASENO = 0;
String RX = "";
int curLine = 0;
String tmpKEY = "";
byte keyCount = 0;
byte failCount = 0;
String secureKey = "00000000";

//For TFT 1.8"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

#define TFT_CS        11
#define TFT_RST        12 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         13
//For Mega2560 SCK=52, SDA(MOSI)=51

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//OTHER
byte LineHeight = 9;
byte textWidth = 7;
#define PC_POWER  30
#define PC_ACC  31

//RGB565
int TFT_TEXT_TITLE = 0xBFC2;
int TFT_TEXT_TITLELINE = 0xFB40;
int TFT_TEXT_HEADER = 0xCFE6;
int TFT_TEXT_SUBLINE = 0xF8A2;
int TFT_TEXT_CONTENT = 0xEFFF;
int TFT_TEXT_KEY = 0xB77D;
int TFT_TEXT_VALUE = 0x97F8;
int TFT_TEXT_PROGREESS = 0xFC49;
int TFT_TEXT_NOTIFY = 0xCFC5;
int TFT_TEXT_CAUTION = 0xFE00;
int TFT_TEXT_WARNING = 0xFA00;
int TFT_TEXT_SUCCESS = 0x77E0;

int TFT_TEXT_RELAY_ON = 0x87A7;
int TFT_TEXT_RELAY_OFF = 0xF1E7;

int TFT_TEMP_GREEN = 0x3FA7;
int TFT_TEMP_YELLOW = 0xF7F0;
int TFT_TEMP_ORANGE = 0xFCE0;
int TFT_TEMP_RED = 0xFA60;
int TFT_TEMP_HIGHRED = 0xF800;

//Procedure Set for TFT Display
void TFT_Cls(){
  //Clear screen
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(false);
  tft.setRotation(3);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
}
void TFT_Title(){
  tft.setTextColor(TFT_TEXT_TITLE);
  tft.println("FzCTRL.sys | Unit-107");
  tft.setTextColor(TFT_TEXT_TITLELINE);
  tft.println("==============================");
  tft.setTextColor(TFT_TEXT_CONTENT);
  curLine = 2;
}

void TFT_SubLine(){
  tft.setTextColor(TFT_TEXT_SUBLINE);
  tft.setTextSize(1);
  tft.println("------------------------------");
  tft.setTextColor(TFT_TEXT_CONTENT);
}

void setup(){
  //Relay
  for(int i = 0; i < relayCount; i++){
    relayPIN[i] = 30+i;
    pinMode(relayPIN[i],OUTPUT);
    digitalWrite(relayPIN[i],HIGH);
  }
  Serial3.begin(115200);
  while (!Serial3) {
    ; // wait for Serial3 port to connect. Needed for native USB
  }
  
  //DS18B20
  //DS18B20.begin();

  //Buzzer
  pinMode(BuzzerPin, OUTPUT);
  digitalWrite(BuzzerPin, LOW);

  pinMode(PC_POWER,OUTPUT);
  pinMode(PC_ACC,OUTPUT);
  digitalWrite(PC_POWER,HIGH);
  digitalWrite(PC_ACC,HIGH);
  
  //Init TFT
  tft.initR(INITR_BLACKTAB);  
  TFT_Cls();  
  TFT_Title();

  //Bootup sequence
  tft.setTextColor(TFT_TEXT_CONTENT);
  tft.print("MEGA2560 Status");
  tft.setCursor(133,curLine*LineHeight);
  tft.setTextColor(TFT_TEXT_SUCCESS);
  tft.println("OK");
  curLine += 1;
  tone(BuzzerPin,1200,200);
  delay(300);
  Serial3.write("BOOTUP|0|-|");
}

void loop(){
  //SERIAL COM.
  if(Serial3.available()){
    while (Serial3.available()) {
      //Read RX Data
      RX += char(Serial3.read());
      delay(10);
    }
  }

  //Decrypt RX
  //Structure : Header|Datacount|Datadelimiter|Content
  if(RX != ""){
    String RXBlock[4];
    StringSplit(RX,'|',RXBlock,4);
    
    String Hdr = RXBlock[0];
    int dataCount = RXBlock[1].toInt();
    char dataDelimeter = RXBlock[2].charAt(0);

    if(Hdr == "BOOTUPSTAT"){//ESP Bootup status
      if(curLine == 10){
        curLine = 0;
        TFT_Cls();  
        TFT_Title();
      }
      
      String Data = RXBlock[3];
      String dataBlock[dataCount];      
      StringSplit(Data,dataDelimeter,dataBlock,dataCount);

      tft.setTextColor(TFT_TEXT_CONTENT);
      tft.setCursor(0,curLine*LineHeight);
      tft.print(dataBlock[0]);      
      if(dataBlock[1] == "OK" || dataBlock[1] == "PASS"){
        tft.setTextColor(TFT_TEXT_SUCCESS);
      }else{
        tft.setTextColor(TFT_TEXT_WARNING);
      }
      tft.setCursor(133,curLine*LineHeight);
      tft.println(dataBlock[1]);
      curLine += 1;
      tone(BuzzerPin,1700,90);

    }else if(Hdr == "PRINT"){//Display message on TFT
      String Data = RXBlock[3];
      String dataBlock[dataCount];      
      StringSplit(Data,dataDelimeter,dataBlock,dataCount);

      tft.setTextColor(TFT_TEXT_NOTIFY);
      tft.print(dataBlock[0]);

    }else if(Hdr == "PRINTLN"){//Display message on TFT and Newline
      if(curLine == 10){
        curLine = 0;
        TFT_Cls();  
        TFT_Title();
      }
      
      String Data = RXBlock[3];
      String dataBlock[dataCount];   
      StringSplit(Data,dataDelimeter,dataBlock,dataCount);

      tft.setTextColor(TFT_TEXT_NOTIFY);
      tft.setCursor(0,curLine*LineHeight);
      tft.println(dataBlock[0]);
      curLine += 1;

    }else if(Hdr == "CLR"){//Display message on TFT and Newline
      curLine = 0;
      TFT_Cls();  
      TFT_Title();
      
    }else if(Hdr == "PHASENO"){//Set PhaseNo
      PHASENO = RXBlock[3].toInt();

      if(PHASENO == 0){
        RX = "";
        TempUpdateCount = 0;
        tone(BuzzerPin,1200,100);
      }else if(PHASENO == 1){
        tone(BuzzerPin,1300,100);
      }else if(PHASENO == 2){
        tone(BuzzerPin,1700,300);
      }
      
    }else if(Hdr == "BUZZER"){
      String Data = RXBlock[3];
      String dataBlock[dataCount];      
      StringSplit(Data,dataDelimeter,dataBlock,dataCount);
      tone(BuzzerPin,dataBlock[0].toInt(),dataBlock[1].toInt());
      
    }else if(Hdr == "PPLOGIN"){
      prepareLOGIN();
    }

    RX = "";
  }
  
  char Key = customKeypad.getKey();
  if(Key){
    
    if(PHASENO == 1){ //LOGIN      
      if(Key == 'C'){//CLS Keys
        tmpKEY = "";
        if(failCount < 3) tone(BuzzerPin,1300,150);
        processLOGIN(false);
        keyCount = 0;

      }else if(Key == 'E'){ //ENTER
        if(tmpKEY == secureKey){
          PHASENO = 2;
          Serial3.write("PHASENO|1|-|1");//Send PhaseNo to ESP-8266
          delay(100);
          tmpKEY = "";
          keyCount = 0;
          failCount = 0;
          PC_SYSTEM_STATUS();
          
        }else{
          failCount++;
          if(failCount < 3){
            processLOGIN();
            
          }else{
            TFT_Cls();
            TFT_Title();   
            tft.println("Process LOGIN sequence.");
            TFT_SubLine();
            tft.setTextColor(TFT_TEXT_NOTIFY);
            tft.println("Please enter SecurePIN");
            tft.setTextColor(TFT_TEXT_CONTENT);
            tft.print("\n");
            tone(BuzzerPin,2100,12000);
            tft.setTextColor(TFT_TEMP_RED);
            tft.setCursor(2,70);
            tft.setTextSize(2);
            tft.println("SYSTEM LOCKED");
            tft.setCursor(35,90);            
          }          
        }
        tmpKEY = "";
        keyCount = 0;
        
      }else{
        if(keyCount < 8){
          tmpKEY += Key;
          keyCount++;
          if(failCount < 3) tone(BuzzerPin,1000,50);
          tft.print("*");
          delay(10);
        }        
      }
         
    }else if(PHASENO == 2){ //System Status
      if(Key == 'E'){ //ENTER
        PHASENO = 3;
        PC_RELAY_STATUS();
      }
      
    }else if(PHASENO == 3){ //Relay Status
      tone(BuzzerPin,1300,30);
      if(Key == 'E'){
        PHASENO = 1;
        processLOGIN();
      }else{
        for(int i = 0; i < relayCount; i++){
          if(relayKey[i] == Key){
            digitalWrite(relayPIN[i],not digitalRead(relayPIN[i]));
          }
        }
        showRelayStatus();
      }
    }
  }
}

int StringSplit(String Source, char Delimeter, String Params[], int MaxParams){  
  int ParamCount = 0;
  int sPos = 0;
  int dPos = 0;

  do{
    dPos = Source.indexOf(Delimeter,sPos);
    if(sPos != -1){
      Params[ParamCount] = Source.substring(sPos, dPos);
    }else{
      Params[ParamCount] = Source.substring(sPos, Source.length());
      break;
    }
    sPos = dPos+1;
    ParamCount++;
    if(ParamCount > MaxParams) break;
  }while(dPos >= 0);

  return ParamCount;
}

void prepareLOGIN(){
  tft.setTextColor(TFT_TEXT_NOTIFY);
  tft.println();
  tft.println("Preparing LOGIN sequence");
  tft.setTextColor(TFT_TEXT_CONTENT);
  delay(2000);
  
  PHASENO = 1;
  Serial3.write("PHASENO|1|-|1");//Send PhaseNo to ESP-8266
  delay(100);
  
  processLOGIN();
}

void processLOGIN(){
  processLOGIN(true);
}

void processLOGIN(boolean beep){  
  TFT_Cls();
  TFT_Title();
  tft.setCursor(0,25);
  tft.println("PROCESS LOGIN SEQUENCE");
  TFT_SubLine();
  tft.setTextColor(TFT_TEXT_NOTIFY);
  tft.println("Please enter SecurePIN");
  tft.setTextColor(TFT_TEXT_CONTENT);
  tft.print("\n");
  if(beep) tone(BuzzerPin,1700,200);
  tft.setCursor(7,70);
  tft.setTextColor(TFT_TEMP_YELLOW);
  tft.setTextSize(3);
}

void PC_SYSTEM_STATUS(){
  TFT_Cls();
  TFT_Title();
  tft.setCursor(0,25);
  tft.setTextColor(TFT_TEXT_HEADER);
  tft.println("SYSTEM STATUS");
  TFT_SubLine();
  tone(BuzzerPin,1700,200);
  delay(1200);
  tft.setTextColor(TFT_TEXT_CONTENT);
  tft.print("MEGA2560                ");
  tft.setTextColor(TFT_TEXT_SUCCESS);
  tft.println("OK");
  TFT_SubLine();
  tft.setTextColor(TFT_TEXT_CONTENT);
  tft.print("ESP8266                 ");
  tft.setTextColor(TFT_TEXT_SUCCESS);
  tft.println("OK");
  TFT_SubLine();
  tft.setTextColor(TFT_TEXT_CONTENT);
  tft.print("WEBSERVER           ");
  tft.setTextColor(TFT_TEXT_SUCCESS);
  tft.println("ONLINE");
  TFT_SubLine();
  tft.setTextColor(TFT_TEXT_CONTENT);
  tft.print("CONTROL PANEL       ");
  tft.setTextColor(TFT_TEXT_SUCCESS);
  tft.println("ONLINE");
  TFT_SubLine();
  tft.setTextColor(TFT_TEXT_CAUTION);
  tft.println("press Enter to continue.");
  tft.setTextColor(TFT_TEXT_CONTENT);
  tone(BuzzerPin,2200,1200);
}

void PC_RELAY_STATUS(){  
  showRelayStatus();
}

void showRelayStatus(){
  TFT_Cls();
  TFT_Title();
  tft.setCursor(0,25);
  tft.setTextColor(TFT_TEXT_HEADER);
  tft.println("RELAY STATUS");
  TFT_SubLine();
  tft.print("  ");
  tone(BuzzerPin,1700,200);  
  tft.setTextColor(TFT_TEXT_WARNING);
  for(int i = 0; i < relayCount; i++){
    if(i > 0 && i % 4 == 0){
      tft.print("\n\n  ");
    }
    if(digitalRead(relayPIN[i]) == LOW){
      tft.setTextColor(TFT_TEXT_RELAY_ON);
      tft.print("[ON-] ");
    }else{
      tft.setTextColor(TFT_TEXT_RELAY_OFF);
      tft.print("[OFF] ");
    }
  }
  tft.setTextColor(TFT_TEMP_YELLOW);
  tft.print("[EXIT]\n");
  TFT_SubLine();
}

void setTempColor(float Temp){
  
  if(Temp > 90){
    tft.setTextColor(TFT_TEMP_HIGHRED);
  }else if(Temp > 80){
    tft.setTextColor(TFT_TEMP_RED);
  }else if(Temp > 60){
    tft.setTextColor(TFT_TEMP_ORANGE);
  }else if(Temp > 40){
    tft.setTextColor(TFT_TEMP_YELLOW);
  }else{
    tft.setTextColor(TFT_TEMP_GREEN);
  }
}
