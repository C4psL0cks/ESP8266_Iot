//https://script.google.com/macros/s/AKfycbxKZ9NW681mC5GoxLFbB-bGA4eqXKDh7Um0YgVNcH5k2P9DNJAF/exec
//http://api.pushingbox.com/pushingbox?devid=v9BAE38FD0936517&humidityData=22&celData=33&fehrData=111&hicData=22&hifData=775
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

#define LEDPIN 2
const char* WIFISSID = "6021607";
const char* PASSWORD = "17401449";
const char* host = "api.pushingbox.com";
const String devid = "v9BAE38FD0936517";
int value;
int timeSinceLastRead = 0;
WiFiClient client;
void connect()
{
  delay(10);
  Serial.println();
  Serial.println();
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

void setup() {

  Serial.begin(115200);
  Serial.setTimeout(2000);
  pinMode(LEDPIN,OUTPUT);
  digitalWrite(LEDPIN,LOW);
  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");
  connect();
}

void loop() {
  bool toReconnect = false;
  if (toReconnect) {
    connect();
  }
  //////// DHT
  if (timeSinceLastRead > 2000) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);

    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }
    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");
    report(h, t, f, hic, hif);

    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100;
}
void report(double humidity, double tempC, double tempF, double heatIndexC, double heatIndexF) {

  delay(30000);
  ++value;
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
  digitalWrite(LEDPIN,HIGH);
  delay(1000);
  Serial.println("closing connection");  
  digitalWrite(LEDPIN,LOW);

}

