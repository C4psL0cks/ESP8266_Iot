#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* WIFI_SSID = "6021607";
const char* WIFI_PASS = "17401449";

void connect() {
  // Connect to Wifi.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to.. ");
  Serial.println(WIFI_SSID);

  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long wifiConnectStart = millis();

  while (WiFi.status() != WL_CONNECTED) {
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
    if (millis() - wifiConnectStart > 10000) {
      Serial.println("Failed to connect to WiFi");
      Serial.println("Please attempt to send updated configuration parameters.");
      return;
    }
  }
  Serial.println();
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();


}
void setup() {

  Serial.begin(9600);
  Serial.setTimeout(2000);
  while (!Serial) { }

  connect();
}

void loop() {


  if(WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
    JsonObject& JSONencoder = JSONbuffer.createObject();

    JSONencoder["sensorType"] = "Temperature";
    
    JSONencoder["humidity"] = "1";
    JSONencoder["tempC"] = "2";
    JSONencoder["tempF"] = "3";
    JSONencoder["heatIndexC"] = "4";
    JSONencoder["heatIndexF"] = "5";

  
    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);

    HTTPClient http;    //Declare object of class HTTPClient

    http.begin("http://439ad51f.ngrok.io/code/node/test.php");      //Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type header

    int httpCode = http.POST(JSONmessageBuffer);   //Send the request
    String payload = http.getString();                                        //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload

    http.end();  //Close connection

  } else {

    Serial.println("Error in WiFi connection");

  }

  delay(10000);  //Send a request every 30 seconds

}
