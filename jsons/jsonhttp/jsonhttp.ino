#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

void setup() {

  Serial.begin(115200);                            //Serial connection
  WiFi.begin("YourNetworkName", "YourPassword");   //WiFi connection

  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion

    delay(500);
    Serial.println("Waiting for connection");

  }

}

void loop() {

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
    JsonObject& JSONencoder = JSONbuffer.createObject();

    JSONencoder["sensorType"] = "Temperature";

    JsonArray& values = JSONencoder.createNestedArray("values"); //JSON array
    values.add(20); //Add value to array
    values.add(21); //Add value to array
    values.add(23); //Add value to array

    JsonArray& timestamps = JSONencoder.createNestedArray("timestamps"); //JSON array
    timestamps.add("10:10"); //Add value to array
    timestamps.add("10:20"); //Add value to array
    timestamps.add("10:30"); //Add value to array

    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);

    HTTPClient http;    //Declare object of class HTTPClient

    http.begin("http://anteph.pythonanywhere.com/postjson");      //Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type header

    int httpCode = http.POST(JSONmessageBuffer);   //Send the request
    String payload = http.getString();                                        //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload

    http.end();  //Close connection

  } else {

    Serial.println("Error in WiFi connection");

  }

  delay(30000);  //Send a request every 30 seconds

}
