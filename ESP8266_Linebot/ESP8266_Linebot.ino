#include <WiFiClientSecure.h>
#include <HTTPClient.h>
const char* ssid     = "wifissid";
const char* password = "password";
WiFiClientSecure client;
int relay1 = 23; /*output pin1*/
int relay2 = 22; /*output pin2*/


void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1,  1); /*status when first start*/
  digitalWrite(relay2,  1); /*1 = out.vcc 0 = out.gnd*/
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(600);
  }
  Serial.print("conected to ");
  Serial.println(ssid);

}

void loop() {
  String url = "http://esp32msg.herokuapp.com/esp32.txt"; //ไม่ต้องเปลี่ยน สำหรับทดลอง
  Serial.println();
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode == 200) {
    String content = http.getString();
    Serial.println(content);

    int Valueopend = -1;
    int Valueclosed = -1;
    int ValueError = -1;
    int Valuetester = -1;

    ValueError = content.indexOf("error");
    Valueopend = content.indexOf("ประตูจงเปิด");
    Valueclosed = content.indexOf("ประตูจงปิด");
    Valuetester = content.indexOf("ทดสอบ");

    if (ValueError > 0) {
      delay(9000);
    }
    else {
      Serial.print("   Open Status : ");
      Serial.print(Valueopend);
      Serial.print("   Close Status : ");
      Serial.print(Valueclosed);
      Serial.print("");

      if (Valueopend and Valueclosed and Valuetester == -1)
      {
        Serial.print("\nAwaiting Command\n");
        digitalWrite(relay1,  1);
        digitalWrite(relay2,  1);
      }

      if (Valueopend != -1)
      {
        digitalWrite(relay1,  1);
        digitalWrite(relay2,  0);
        Serial.print("\nDoor Open\n");
        delay(1000);
        digitalWrite(relay1,  1);
        digitalWrite(relay2,  1);
      }
      if (Valueclosed != -1)
      {
        digitalWrite(relay1,  0);
        digitalWrite(relay2,  1);
        Serial.print("\nDoor Close\n");
        delay(1000);
        digitalWrite(relay1,  1);
        digitalWrite(relay2,  1);
      }


      if (Valuetester != -1)
      {
        digitalWrite(relay1,  1);
        digitalWrite(relay2,  0);
        Serial.print("\nTest Door Open");
        delay(1000);
        digitalWrite(relay1,  1);
        digitalWrite(relay2,  1);
        delay(1000);
        digitalWrite(relay1,  0);
        digitalWrite(relay2,  1);
        Serial.print("\nTest Door Close");
        delay(1000);
        digitalWrite(relay1,  1);
        digitalWrite(relay2,  1);
      }

    }
  } else {
    Serial.println("Fail. error code " + String(httpCode));
  }
  delay(1000);
}
