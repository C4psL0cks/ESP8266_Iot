//https://script.google.com/macros/s/AKfycbxKZ9NW681mC5GoxLFbB-bGA4eqXKDh7Um0YgVNcH5k2P9DNJAF/exec
//http://api.pushingbox.com/pushingbox?devid=v9BAE38FD0936517&humidityData=22&celData=33&fehrData=111&hicData=22&hifData=775

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* WIFISSID = "6021607";
const char* PASSWORD = "17401449";
const char* host = "api.pushingbox.com";
const String devid = "v9BAE38FD0936517";
WiFiClient client;

void setup() {

  Serial.begin(115200);
  Serial.setTimeout(2000);
  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");
  Serial.print("Connecting to ");
  Serial.println(WIFISSID);

  WiFi.begin(WIFISSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {



  int value1 = 6;
  int value2 = 7;
  int value3 = 8;
  int value4 = 9;
  int value5 = 0;
  report(value1, value2, value3, value4, value5);
  delay(1000);
}
void report(int value1, int value2, int value3, int value4, int value5) {

  //http://api.pushingbox.com/pushingbox?devid=vB388D4B1A917188&value1=55&value2=55&value3=55&value4=55&value5=55
  String Host = "api.pushingbox.com";
  String Deviceid = "vB388D4B1A917188";


  Serial.print("connecting to " + String(Host));
  if (!client.connect(Host, 80)) {
    Serial.println("connection failed");
    return;
  }
  String url = "/pushingbox?devid=" + Deviceid + "&value1=" + value1 + "&value2=" + value2 + "&value3=" + value3 + "&value4=" + value4 + "&value5=" + value5;
  Serial.print("Requesting URL: " + url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + Host + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
}
