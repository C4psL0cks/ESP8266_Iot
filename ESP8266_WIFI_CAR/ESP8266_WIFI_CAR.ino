#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
#if !(defined ESP8266 )
#error Please select the ArduCAM ESP8266 UNO board in the Tools/Board
#endif

//This demo can only work on OV2640_MINI_2MP or ARDUCAM_SHIELD_V2 platform.
#if !(defined (OV2640_MINI_2MP)||defined (OV5640_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP_PLUS) \
    || defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) \
    ||(defined (ARDUCAM_SHIELD_V2) && (defined (OV2640_CAM) || defined (OV5640_CAM) || defined (OV5642_CAM))))
#error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif
// set GPIO16 as the slave select :

#define MAX_FRAME_LENGTH 256 // Here we define a maximum framelength to 64 bytes. Default is 256.

const int CS = D3;


//you can change the value of wifiType to select Station or AP mode.
//Default is AP mode.
int wifiType = 0; // 0:Station  1:AP

//AP mode configuration
//Default is arducam_esp8266.If you want,you can change the AP_aaid  to your favorite name
const char *AP_ssid = "arducam_esp8266";
//Default is no password.If you want to set password,put your password here
const char *AP_password = "";

//Station mode you should put your ssid and password
const char *ssid = "CarV1"; // Put your SSID here
const char *password = "12345678"; // Put your PASSWORD here

static const size_t bufferSize = 4096;  //4096;
static uint8_t buffer[bufferSize] = {0xFF};
uint8_t temp = 0, temp_last = 0;
int i = 0;
bool is_header = false;
int ledState = LOW;

ESP8266WebServer server(80);

ArduCAM myCAM(OV2640, CS);


void start_capture() {
  myCAM.flush_fifo();
  myCAM.clear_fifo_flag();
  myCAM.start_capture();
}

void camCapture(ArduCAM myCAM) {
  WiFiClient client = server.client();
  uint32_t len  = myCAM.read_fifo_length();

  if (len >= 393216)    // if (len >= MAX_FIFO_SIZE) //8M
  {
    //serial.println(F("Over size."));
    return;
  }
  if (len == 0 ) //0 kb
  {
    //serial.println(F("Size is 0."));
    return;
  }
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
  SPI.transfer(0xFF);       //***********************
  if (!client.connected()) return;
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: image/jpeg\r\n";
  response += "Content-len: " + String(len) + "\r\n\r\n";
  server.sendContent(response);
  i = 0;
  while (len)
  { size_t will_copy = (len < bufferSize) ? len : bufferSize;
    myCAM.transferBytes(&buffer[0], &buffer[0], will_copy);
    if (!client.connected()) break;
    client.write(&buffer[0], will_copy);
    //Serial.write(&buffer[0], will_copy);
    len -= will_copy;
  }
  myCAM.CS_HIGH();
}

void serverCapture() {
  start_capture();
  //serial.println(F("CAM Capturing"));

  int total_time = 0;

  total_time = millis();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  total_time = millis() - total_time;
  //serial.print(F("capture total_time used (in miliseconds):"));
  //serial.println(total_time, DEC);

  total_time = 0;

  //serial.println(F("CAM Capture Done."));
  total_time = millis();
  camCapture(myCAM);
  total_time = millis() - total_time;
  //serial.print(F("send total_time used (in miliseconds):"));
  //serial.println(total_time, DEC);
  //serial.println(F("CAM send Done."));
}

void serverStream() {
  myCAM.set_bit(ARDUCHIP_GPIO, GPIO_PWDN_MASK); //enable low power
  WiFiClient client = server.client();

  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (1) {
    myCAM.clear_bit(ARDUCHIP_GPIO, GPIO_PWDN_MASK); //disable low power
    delay(5);
    start_capture();
    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
    size_t len = myCAM.read_fifo_length();
    //Serial.println(">>Length: " + len);
    if (len >= 393216) {
      Serial.println("Over size.");
      continue;
    } else if (len == 0 ) {
      Serial.println("Size is 0.");
      continue;
    }

    myCAM.CS_LOW();
    myCAM.set_fifo_burst();
    SPI.transfer(0xFF);
    if (!client.connected()) break;
    response = "--frame\r\n";
    response += "Content-Type: image/jpeg\r\n\r\n";
    server.sendContent(response);

    //    static const size_t bufferSize = 512; //4096;
    //    static uint8_t buffer[bufferSize] = {0xFF};

    while (len) {
      size_t will_copy = (len < bufferSize) ? len : bufferSize;
      myCAM.transferBytes(&buffer[0], &buffer[0], will_copy);
      if (!client.connected()) break;
      client.write(&buffer[0], will_copy);
      len -= will_copy;
    }
    myCAM.CS_HIGH();
    myCAM.set_bit(ARDUCHIP_GPIO, GPIO_PWDN_MASK); //enable low power
    if (!client.connected()) break;
  }
}
void handleNotFound() {
  String message = "Server is running!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();

  message += "\n\n /capture : camera capture";
  message += "\n /stream : video streaming";
  message += "\n\n /?ql=0 : 320x240";
  message += "\n /?ql=1 : 640x480";
  message += "\n /?ql=2 : 1280x720";
  message += "\n /?ql=3 : 1920x1080";
  message += "\n /?ql=4 : 2048x1563";
  message += "\n /?ql=5 : 2592x1944";
  message += "\n";
  server.send(200, "text/plain", message);

  if (server.hasArg("ql")) {
    myCAM.clear_bit(ARDUCHIP_GPIO, GPIO_PWDN_MASK); //disable low power
    int ql = server.arg("ql").toInt();
    myCAM.OV2640_set_JPEG_size(ql);
    myCAM.set_bit(ARDUCHIP_GPIO, GPIO_PWDN_MASK); //enable low power
    Serial.println("QL change to: " + server.arg("ql"));
  }
}

void setup() {
  uint8_t vid, pid;
  uint8_t temp;
#if defined(__SAM3X8E__)
  Wire1.begin();
#else
  Wire.begin();
#endif
  // serial.begin(115200);
  //  Serial.println(F("ArduCAM Start!"));

  // set the CS as an output:
  pinMode(CS, OUTPUT);
  pinMode(D4, OUTPUT);
  // initialize SPI:
  SPI.begin();
  SPI.setFrequency(4000000); //4MHz

  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55) {
    //serial.println(F("SPI1 interface Error!"));
    while (1);
  }

  myCAM.wrSensorReg8_8(0xff, 0x01);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  //  if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 )))
  //serial.println(F("Can't find OV2640 module!"));
  //  else
  //serial.println(F("OV2640 detected."));

  myCAM.set_format(JPEG);
  myCAM.InitCAM();
  myCAM.OV2640_set_JPEG_size(OV2640_352x288); //160x120 176x144 320x240 352x288 640x480 800x600 1024x768 1280x1024 1600x1200
  myCAM.clear_fifo_flag();
  if (wifiType == 0) {
    if (!strcmp(ssid, "SSID")) {
      //serial.println(F("Please set your SSID"));
      while (1);
    }
    if (!strcmp(password, "PASSWORD")) {
      //serial.println(F("Please set your PASSWORD"));
      while (1);
    }
    // Connect to WiFi network
    //serial.println();
    //serial.println();
    //serial.print(F("Connecting to "));
    //serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }

      // set the LED with the ledState of the variable:
      digitalWrite(D4, ledState);
      //serial.print(F("."));
    }
    //serial.println(F("WiFi connected"));
    //serial.println("");
    //serial.println(WiFi.localIP());
  } else if (wifiType == 1) {
    //serial.println();
    //serial.println();
    //serial.print(F("Share AP: "));
    //serial.println(AP_ssid);
    //serial.print(F("The password is: "));
    //serial.println(AP_password);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_ssid, AP_password);
    //serial.println("");
    //serial.println(WiFi.softAPIP());
  }

  // Start the server
  //  server.on("/capture", HTTP_GET, serverCapture);
  server.on("/stream", HTTP_GET, serverStream);
  server.onNotFound(handleNotFound);
  server.begin();
  //serial.println(F("Server started"));
  digitalWrite(D4, LOW);
}

void loop() {

  server.handleClient();
}

