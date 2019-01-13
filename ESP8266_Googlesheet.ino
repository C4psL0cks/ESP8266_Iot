//https://script.google.com/macros/s/AKfycbxKZ9NW681mC5GoxLFbB-bGA4eqXKDh7Um0YgVNcH5k2P9DNJAF/exec
//http://api.pushingbox.com/pushingbox?devid=v9BAE38FD0936517&humidityData=22&celData=33&fehrData=111&hicData=22&hifData=775
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
 
  
    float h = 4;
    float t = 5;
    float f = 5;
    float hif = 6;
    float hic = 6;
    report(h, t, f, hic, hif);
    delay(1000);
}
void report(double humidity, double tempC, double tempF, double heatIndexC, double heatIndexF) {

  
  Serial.print("connecting to ");
  Serial.println(host);

  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  String url = "/pushingbox?devid=" + devid
               + "&humidityData=" + humidity
               + "&celData=" + tempC
               + "&fehrData=" + tempF
               + "&hicData=" + heatIndexC
               + "&hifData=" + heatIndexF;

  Serial.print("Requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url
               + " HTTP/1.1\r\n"
               + "Host: " + host + "\r\n"
               + "Connection: close\r\n\r\n"
              );
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
}

