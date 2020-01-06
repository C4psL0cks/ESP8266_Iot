#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
WiFiClientSecure wifiClient;

const char* ssid = "6021607";                  // Your wifi Name
const char* password = "17401449";          // Your wifi Password
const char *host = "192.168.0.5"; //Your pc or server (database) IP, example : 192.168.0.0 , if you are a windows os user, open cmd, then type ipconfig then look at IPv4 Address.

void setup() {

  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(250);
  }

  Serial.println("");
  Serial.println("Connected to Network/SSID");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

void loop() {
  // put your main code here, to run repeatedly:
  HTTPClient http;    //Declare object of class HTTPClient

  String LdrValueSend, postData;
  int ldrvalue = 10;
  LdrValueSend = String(ldrvalue);   //String to interger conversion

  //Post Data
  postData = "ldrvalue=" + LdrValueSend;

  http.begin("http://192.168.0.5/Nodemcu_db_record_view/InsertDB.php");              //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header

  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload

  //Serial.println("LDR Value=" + ldrvalue);
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  Serial.println("LDR Value=" + LdrValueSend);
  http.end();  //Close connection
  delay(5000);
}
