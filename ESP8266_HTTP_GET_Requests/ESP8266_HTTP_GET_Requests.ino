#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SSID "--------------------"
#define PASSWORD "--------------------"

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n", SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
}
void loop() {

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    HTTPClient http;                                           //Declare an object of class HTTPClient
    http.begin("http://jsonplaceholder.typicode.com/users/1"); //Specify request destination
    int httpCode = http.GET();                                 //Send the request
    if (httpCode > 0)
    { //Check the returning code
      String payload = http.getString(); //Get the request response payload
      Serial.println(payload);           //Print the response payload
    }
    http.end(); //Close connection
  }
  delay(30000); //Send a request every 30 seconds
}
