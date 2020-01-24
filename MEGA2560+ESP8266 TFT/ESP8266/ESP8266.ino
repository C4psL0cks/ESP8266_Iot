//For ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

ESP8266WebServer server(80);
const char* SoftAPID = "MEGAESP-CFG.1107A" ; //AP Name for config
const char* SoftAPPass = "00000000";
const String systemPass = "15839521";
String st, content;
int statusCode;

//SERIAL COM.
String RX = ""; // for incoming serial data
String outgoingData = ""; // for outgoingData serial data
String dBuff = "";

//OTHER
byte PHASENO = 0;
boolean HandleClient = 0;
String esid, epass;
//================= END ===============

void setup() {
  // Begin the Serial at 115200 Baud
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  delay(1200);
  Serial.print("BOOTUPSTAT|2|,|ESP8266 Connection,OK");
  delay(700);
}

void Bootup(){
  //Init ESP8266
  EEPROM.begin(512);
  delay(100);
  
  // read eeprom for ssid and pass
  delay(700); 
  for(int i = 0; i < 32; ++i){
    esid += char(EEPROM.read(i));
    delay(10);
  }  
  for(int i = 32; i < 96; ++i){
    epass += char(EEPROM.read(i));
    delay(10);
  }
  Serial.print("BOOTUPSTAT|2|,|ESP-EEPROM Reading,OK");
  delay(700);
  Serial.print("PRINTLN|1|,|");
  delay(700);
  Serial.print("PRINTLN|1|,|Connecting to WiFi...");
  delay(700);
  WiFi.begin(esid.c_str(), epass.c_str());
  if(testWifi()) {
    launchWeb(0);
    delay(700);
    return;
  }
  Serial.print("BOOTUPSTAT|2|,|ESP-WiFi Status,FAIL");
  delay(700);
  Serial.print("PRINTLN|1|,|Setting up AP...");
  setupAP();
  Serial.print("BOOTUPSTAT|2|,|ESP-AP Status,OK");
  delay(700);  
}

bool testWifi(void) {
  
  int c = 0;
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED){
      Serial.print("BOOTUPSTAT|2|,|ESP-WiFi Connection,OK");
      delay(700);    
      return true; 
    } 
    delay(700);
    c++;
  }
  return false;
}

void launchWeb(int webtype) {
  Serial.print("PRINTLN|1|,|Creating WebServer...");
  delay(700);
  createWebServer(webtype);
  
  // Start the server
  server.begin();
  Serial.print("BOOTUPSTAT|2|,|ESP-Server Status,OK");
  delay(700);
  HandleClient = 1;
  Serial.print("PPLOGIN|0|-|1");
}

void setupAP(void) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.print("PRINTLN|1|,|Scanning Network...");
  delay(700);
  int n = WiFi.scanNetworks();
  Serial.print("CLR|1|,|");
  delay(700);
  Serial.print("BOOTUPSTAT|2|,|ESP-Scan network,OK");
  delay(700);
  Serial.printf("PRINTLN|1|,|%d Network(s) found.",n);
  delay(700);
  st = "<ul type='square'>";
  for (int i = 0; i < n; ++i){
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);
    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
    st += "</li>";

    Serial.print("BOOTUPSTAT|2|,|" + WiFi.SSID(i) + "," + WiFi.RSSI(i));
    delay(700);    
  }
  st += "</ul>";
  delay(7000);
  Serial.print("CLR|1|,|");
  delay(700);
  WiFi.softAP(SoftAPID , SoftAPPass, 6);
  launchWeb(1);
}

void createWebServer(int webtype){
  
  if ( webtype == 1 ) {//============================= Configuration Page =============================
    
    server.on("/", []() {
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      
      content = "<!DOCTYPE HTML>\r\n";
      content += "<html>";
      content += "<head>";
      content += "<meta name='viewport' content='width=device-width, scale=1.0'>";
      content += "</head>";
      content += "<body>";
      content += "<font face='verdana' size=2>";
      content += "=================================<br>";
      content += "<b>ESP8266 Configuration page.</b><br>";
      content += "=================================<br>";
      content += "<br>Available AP List<br>";
      content += "<p>" + st + "</p>";
      content += "=================================<br>";
      content += "<p>";
      content += "<form method='get' action='setting@";
      content += SoftAPPass;
      content += "'>";
      content += "<table border='0'>";
      content += "  <tr>";
      content += "    <td><b>SSID --></b></td>";
      content += "    <td><input name='ssid' size='20'></td>";
      content += "  </tr>";
      content += "  <tr>";
      content += "    <td><b>PASS -></b></td>";
      content += "    <td><input name='pass' size='20'></td>";
      content += "  </tr>";
      content += "  <tr>";
      content += "    <td colspan=2><hr></td>";
      content += "  </tr>";
      content += "  <tr>";
      content += "    <td></td>";
      content += "    <td><input type='submit' size='20' value='Save Settings'></td>";
      content += "  </tr>";
      content += "</table";
      content += "</form>";
      content += "</p>";
      content += "=================================<br>";
      content += "</font>";
      content += "</body>";
      content += "</html>";
      
      server.send(200, "text/html", content); 
      
    });
    
    server.on("/setting@" + String(SoftAPPass), []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        delay(700);
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);      
        }
        delay(10);
        for (int i = 0; i < qsid.length(); ++i){
          EEPROM.write(i, qsid[i]);
          delay(100);
        }
        delay(700);
        for (int i = 0; i < qpass.length(); ++i){
          EEPROM.write(32+i, qpass[i]);
          delay(100);
        }
        delay(10);        
        EEPROM.commit();
        
        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
      }
      server.send(statusCode, "application/json", content);
      ESP.restart();
    });
    
    server.on("/reset@" + String(SoftAPPass), []() {
      content = "<!DOCTYPE HTML>\r\n<html>";
      content += "<p>Clearing the EEPROM</p></html>";
      server.send(200, "text/html", content);
      delay(700);
      
      for (int i = 0; i < 96; ++i) {
        EEPROM.write(i, 0);
        delay(10);
      }
      EEPROM.commit();
      
      content += "<p>Clearing successful.</p></html>";
      server.send(200, "text/html", content);
      WiFi.disconnect();
      ESP.restart();
    });
    
  } else if (webtype == 0) {//============================= Control Page =============================
    
    server.on("/", []() {      
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      
      String macStr = WiFi.macAddress();
      String SSIDStr = SoftAPID;
      String cfgContent = "==============================\n";
      cfgContent += "Configuration successful.\n";
      cfgContent += "==============================\n";
      cfgContent += "[SSID]-> " + SSIDStr + "\n[IP]---> " + ipStr + " \n[MAC]--> " + macStr + "\n";
      cfgContent += "==============================\n";
      
      server.send(200, "x-www-form-urlencoded", cfgContent);
    });
    
    server.on("/reset@" + String(SoftAPPass), []() {      
      content = "<!DOCTYPE HTML>\r\n<html>";
      content += "<p>Clearing the EEPROM</p></html>";
      server.send(200, "text/html", content);
      delay(700);
      for (int i = 0; i < 96; ++i) {
        EEPROM.write(i, 0);
        delay(10);
      }
      EEPROM.commit();
      
      content += "<p>Clearing successful.</p></html>";
      server.send(200, "text/html", content);
      WiFi.disconnect();
      ESP.restart();
    });
    
  }
}

void loop() {
  //ESP-8266
  if(HandleClient){
    server.handleClient();
  }  

  //SERIAL COM.
  if(Serial.available()){
    while (Serial.available()) {
      //Read RX Data
      RX += char(Serial.read());
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

    if(Hdr == "BOOTUP"){//Bootup ESP command      
      Bootup();
    }
    if(Hdr == "RESTART"){//Restart ESP command      
      ESP.restart();
    }
    
    RX = "";    
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

String genString(int Length){
  String src = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijkllmnopqrstuvwxyz0123456789";
  String tmp = "";
  for(int i = 0; i < Length; i++){
    tmp += src.charAt(random(0,src.length()-1));
  }
  return tmp;
}

const char* wl_status_to_mark(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "/";
    case WL_IDLE_STATUS: return ">";
    case WL_NO_SSID_AVAIL: return "-";
    case WL_SCAN_COMPLETED: return "+";
    case WL_CONNECTED: return "*";
    case WL_CONNECT_FAILED: return "!";
    case WL_CONNECTION_LOST: return "^";
    case WL_DISCONNECTED: return "<";
  }
}
