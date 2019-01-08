#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* WIFI_SSID = "6021607";
const char* WIFI_PASS = "17401449";

WiFiClientSecure wifiClient;
HTTPClient http;

void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void loop() {

  float value1 = 3;
  float value2 = 4;
  float value3 = 5;
  report(value1 , value2, value3);
}

void report(double value1, double value2, double value3) {

  StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["value1"] = value1;
  JSONencoder["value2"] = value2;
  JSONencoder["value3"] = value3;

  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);

  Serial.println("Authenticating Device...");

  http.begin("http://maker.ifttt.com/trigger/data/with/key/d-K9LQ5IDy97XIamb7HKPm");                                                                                           //Specify request destination
  http.addHeader("Content-Type", "application/json");                                    //Specify content-type header
  //http.addHeader("Accept", "application/json");
  int httpCode = http.POST(JSONmessageBuffer);                                            //Send the request
  String payload = http.getString();                                                      //Get the response payload
  Serial.print("HTTPS CODE :");
  Serial.println(httpCode);                                                              //Print HTTP return code
  Serial.print("Payload :");
  Serial.print(payload);                                                                 //Print request response payload
  http.end();                                                                            //Print request response payload                                                                         //Close connection
  delay(3000);                                                                          //Send a request every 30 seconds
}
