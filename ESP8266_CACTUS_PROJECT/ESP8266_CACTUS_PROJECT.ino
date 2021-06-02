#include <ESP8266WiFi.h>
#include <SocketIOClient.h>
#include <ArduinoJson.h>

SocketIOClient socket;

#define WIFI_SSID   "dlink-ABD0"
#define WIFI_PASS   "yyafr68490"

String Hostname = "192.168.0.6";
int Port = 3484;

#define sensor_pin A0
int relay =  5;

// value Soil_Moisture_humidity
float humidity_sensor;
float humidity;

//online_status
int online_status = 1;

// device_id
int device_id = 1;
int hudimity_check;

StaticJsonBuffer<300> JSONbuffer;
JsonObject& JSONencoder = JSONbuffer.createObject();
char JSONmessageBuffer[300];

void setup() {

  Serial.begin(115200);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  while (!Serial) continue;
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("Running!");
  Serial.println("-------------------------------------");
  // Connect to Wifi.
  Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("WiFi Connecting to %s\n", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  Serial.println();

  socket.connect(Hostname, Port);

  //get value form node
  // humidity_check
  socket.on("auto", autos);
  // auto
  socket.on("switch", switchs);
  // check
  socket.on("check",checks);

}

void loop() {

  humidity_sensor = 10;
  //  humidity_sensor = ( 100.00 - ( (analogRead(sensor_pin) / 1023.00) * 100.00 ) );
  Serial.print("Soil Moisture = " + String(humidity_sensor) + "%");

  humidity = (humidity_sensor * 100) / 50;
  //Serial.print(humidity);

  JSONencoder["device_id"] = device_id;
  JSONencoder["humidity"] = humidity;
  JSONencoder["online_status"] = online_status;

  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  //Serial.println(JSONmessageBuffer);

  //send data to node.js socket.io
  socket.emit("data", JSONmessageBuffer);

  //moniter data
  if (socket.monitor()) {}

  delay(1000);
}

void autos(String autos){
   Serial.println("[auto] " + autos);
}
void switchs(String switchs){
   Serial.println("[switch] " + switchs);
}
void checks(String checks){
   Serial.println("[check] " + checks);
}

//void auto_water(String control) {
//  Serial.println("[control] " + control);
//  if (control.toInt() == 1) {
//    if (humidity < hudimity_check) {
//      Serial.println("[control] ON");
//      digitalWrite(relay, LOW);
//    }
//    else {
//      digitalWrite(relay, HIGH);
//      Serial.println("[control] OFF");
//    }
//  }
//  else {
//    digitalWrite(relay, HIGH);
//    Serial.println("[control] OFF");
//  }
//}
//
//void check(String switchs) {
//  Serial.println("[switchs] " + switchs);
//  if (switchs.toInt() == 0) {
//    hudimity_check = 0;
//  }
//  else if (switchs.toInt() == 1) {
//    hudimity_check = 10;
//  }
//  else if (switchs.toInt() == 2) {
//    hudimity_check = 20;
//  }
//  else if (switchs.toInt() == 3) {
//    hudimity_check = 30;
//  }
//  else if (switchs.toInt() == 4) {
//    hudimity_check = 40;
//  }
//  else if (switchs.toInt() == 5) {
//    hudimity_check = 50;
//  }
//  else if (switchs.toInt() == 6) {
//    hudimity_check = 60;
//  }
//  else if (switchs.toInt() == 7) {
//    hudimity_check = 70;
//  }
//  else if (switchs.toInt() == 8) {
//    hudimity_check = 80;
//  }
//  else if (switchs.toInt() == 9) {
//    hudimity_check = 90;
//  }
//  else {
//    hudimity_check = 0;
//  }
//}
