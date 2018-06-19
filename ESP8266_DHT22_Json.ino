#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);


const char* WIFI_SSID = "6021607";
const char* WIFI_PASS = "17401449";

WiFiClientSecure wifiClient;

void connect() {

  // Connect to Wifi.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long wifiConnectStart = millis();

  while (WiFi.status() != WL_CONNECTED) {
    // Check to see if
    if (WiFi.status() == WL_CONNECT_FAILED) {
      Serial.println("Failed to connect to WIFI. Please verify credentials: ");
      Serial.println();
      Serial.print("SSID: ");
      Serial.println(WIFI_SSID);
      Serial.print("Password: ");
      Serial.println(WIFI_PASS);
      Serial.println();
    }

    delay(500);
    Serial.print(".");
    // Only try for 10 seconds.
    if (millis() - wifiConnectStart > 10000) {
      Serial.println("Failed to connect to WiFi");
      Serial.println("Please attempt to send updated configuration parameters.");
      return;
    }
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  //while (!Serial) { }

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");

  connect();
}

int timeSinceLastRead = 0;
void loop() {

  bool toReconnect = false;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Disconnected from WiFi");
    toReconnect = true;
  }

  if (toReconnect) {
    connect();
  }

  // Report every 2 seconds.
  if (timeSinceLastRead > 2000) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
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
    report(h, t, f, hic, hif); //send to function

    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100;
}

void report(double humidity, double tempC, double tempF, double heatIndexC, double heatIndexF) {

  if (WiFi.status() == WL_CONNECTED)
  {

    StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
    JsonObject& JSONencoder = JSONbuffer.createObject();


    JSONencoder["humidity"] =  humidity;
    JSONencoder["tempC"] = tempC;
    JSONencoder["tempF"] = tempF;
    JSONencoder["heatIndexC"] = heatIndexC;
    JSONencoder["heatIndexF"] = heatIndexF;


    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    
    Serial.println("Authenticating Device...");
    HTTPClient http;                                                                        //Declare object of class HTTPClient
    http.begin("http://c55514ed.ngrok.io/code/node/inputjson.php");                                                                                           //Specify request destination
    http.addHeader("Content-Type", "application/json");                                     //Specify content-type header
    http.addHeader("Accept", "application/json");

    int httpCode = http.POST(JSONmessageBuffer);                                            //Send the request
    String payload = http.getString();                                                      //Get the response payload
    
    Serial.print("HTTPS CODE :");   
    Serial.println(httpCode);                                                              //Print HTTP return code
    Serial.print("Payload :");   
    Serial.print(payload);                                                                 //Print request response payload
    http.end();                                                                            //Close connection
  }
  else
  {
    Serial.println("Error in WiFi connection");
  }
  delay(3000);                                                                          //Send a request every 30 seconds
  Serial.print("Reported!");

}
