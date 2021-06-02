#include <ESP8266WiFi.h>

#define WIFI_SSID "dlink-ABD0"
#define WIFI_PASS "yyafr68490"
const char *host = "192.168.0.106";
WiFiClient client;

void setup()
{

    Serial.begin(115200);
    Serial.println();
    Serial.println("-------------------------------------");
    Serial.println("Running!");
    Serial.println("-------------------------------------");
    // Connect to Wifi.
    Serial.println();
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.printf("WiFi Connecting to %s\n", WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\nWiFi connected\nIP : ");
    Serial.println(WiFi.localIP());
    Serial.println();
}

void loop()
{

    int temp = 25;
    String url = "/apiesp8266/public/api.php?temperature=";
    url += temp;

    if (!client.connect(host, 80))
    {
        Serial.println("connection failed");
        return;
    }

    Serial.print("Requesting URL: ");
    Serial.println(url);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0)
    {
        if (millis() - timeout > 5000)
        {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }
    // Read all the lines of the reply from server and print them to Serial
    while (client.available())
    {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }
    Serial.println();
    Serial.println("closing connection");
    delay(1000);
}