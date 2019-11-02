#include <time.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set these to run example.
#define FIREBASE_HOST "database-ionic-b75b9.firebaseio.com"
#define FIREBASE_AUTH "UU5t80yfVMjP9AuGfVE3hCAxrvHcUBwbA0QA0wMa"
#define WIFI_SSID "6021607"
#define WIFI_PASSWORD "17401449"

int randNumber1, randNumber2;

/// time
struct tm* p_tm;
char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";
int sec;
String times = "";
bool checksend = false;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("Running!");
  Serial.println("-------------------------------------");
  // Connect to Wifi.
  Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("WiFi Connecting to %s\n", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  Serial.println();

  configTime(7 * 3600, 0, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(100);
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {

  time_t time_now = time(nullptr);   // Time
  p_tm = localtime(&time_now);
  sec = p_tm->tm_min; //tm_min
  times = ctime(&time_now);

  randNumber1 = random(10, 20);
  randNumber2 = random(20, 30);

  // set value
  Firebase.setFloat("Temperature/sensor1", randNumber1);
  Firebase.setFloat("Temperature/sensor2", randNumber2);

  //get value
  Serial.print("servo1: ");
  Serial.println(Firebase.getInt("servo/Range1"));
  Serial.print("servo2: ");
  Serial.println(Firebase.getInt("servo/Range2"));
  Serial.print("servo3: ");
  Serial.println(Firebase.getInt("servo/Range3"));

  //get value
  Serial.print("switch1: ");
  Serial.println(Firebase.getBool("switch/switch1"));
  Serial.print("switch2: ");
  Serial.println(Firebase.getBool("switch/switch2"));

  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /number failed:");
    Serial.println(Firebase.error());
    return;
  }

  //  if ((sec % 3) == 0) {
  //    Serial.println("JUMP:" + String(sec));
  //    if (checksend == false) {
  //      /// send data
  //      Serial.println("SEND");
  //      checksend = true;
  //    }
  //  }
  //  else {
  //    //Serial.println("RESRT:" + String(sec));
  //    checksend = false;
  //  }

  //  // remove value
  //  Firebase.remove("number");
  //  delay(1000);

  // set string value
  //  Firebase.setString("message", "hello world");
  //  // handle error
  //  if (Firebase.failed()) {
  //    Serial.print("setting /message failed:");
  //    Serial.println(Firebase.error());
  //    return;
  //  }
  //  delay(1000);

  // set bool value
  //  Firebase.setBool("switch/switch1", false);
  //  Firebase.setBool("switch/switch2", false);
  //  // handle error
  //  if (Firebase.failed()) {
  //    Serial.print("setting /truth failed:");
  //    Serial.println(Firebase.error());
  //    return;
  //  }



  //  // append a new value to /logs
  //  String name = Firebase.pushInt("logs", n++);
  //  // handle error
  //  if (Firebase.failed()) {
  //    Serial.print("pushing /logs failed:");
  //    Serial.println(Firebase.error());
  //    return;
  //  }
  //  Serial.print("pushed: /logs/");
  //  Serial.println(name);
  delay(50);
}
