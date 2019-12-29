#include <ESP8266WiFi.h>
#include <SocketIOClient.h>
#include <ArduinoJson.h>

SocketIOClient socket;

#define WIFI_SSID   "6021607"
#define WIFI_PASS   "17401449"

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
  socket.on("auto", auto_water);
  socket.on("switchs", water);
  //socket.on("check", check);
  digitalWrite(relay, HIGH);

}

void loop() {

  humidity_sensor = ( 100.00 - ( (analogRead(sensor_pin) / 1023.00) * 100.00 ) );
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
  socket.monitor();


  delay(1000);
}

void check(String check) {
  Serial.println("[check] " + check);
  hudimity_check = check.toInt();
  // auto water

  Serial.println(hudimity_check);

  if (humidity < hudimity_check) {
    Serial.println("[auto-water] ON");
    digitalWrite(relay, LOW);
  }
  //  else {
  //    digitalWrite(relay, HIGH);
  //    Serial.println("[auto-water] OFF");
  //  }
}

void auto_water(String control) {
  Serial.println("[control] " + control);
  if (control.toInt() == 1) {
    if (humidity < 80) {
      Serial.println("[control] ON");
      digitalWrite(relay, LOW);
    }
    //    else {
    //      digitalWrite(relay, HIGH);
    //      Serial.println("[control] OFF");
    //    }
  }
}

void water(String switchs) {
  Serial.println("[switchs] " + switchs);
  if (switchs.toInt() == 1) {
    digitalWrite(relay, LOW);
    Serial.println("[switchs] ON");
  }
  else {
    digitalWrite(relay, HIGH);
    Serial.println("[switchs] OFF");
  }
}
