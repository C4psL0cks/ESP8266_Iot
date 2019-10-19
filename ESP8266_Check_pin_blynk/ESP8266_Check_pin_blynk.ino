#define BLYNK_MAX_SENDBYTES 256
#define BLYNK_PRINT Serial // Enables Serial Monitor
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

int buttonState;
int chanal1 = 20, chanal2 = 6, chanal3 = 6, chanal4 = 20;
int bt1 = 16, bt2 = 5, bt3 = 4, bt4 = 0, bt5 = 2, bt6 = 14, bt7 = 12;

char auth[] = "13336b2942c84b3d9a2cad9400d61085";
#define WIFI_SSID  "6021607"
#define WIFI_PASS  "17401449"

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, WIFI_SSID, WIFI_PASS, "blynk-cloud.com", 8442);
  pinMode(bt1, OUTPUT);
  pinMode(bt2, OUTPUT);
  pinMode(bt3, OUTPUT);
  pinMode(bt4, OUTPUT);
  pinMode(bt5, OUTPUT);
  pinMode(bt6, OUTPUT);
  pinMode(bt7, OUTPUT);
  digitalWrite(bt1, LOW);
  digitalWrite(bt2, LOW);
  digitalWrite(bt3, LOW);
  digitalWrite(bt4, LOW);
  digitalWrite(bt5, LOW);
  digitalWrite(bt6, LOW);
  digitalWrite(bt7, LOW);

}
BLYNK_WRITE(V5) {
  buttonState = param.asInt();
  if (buttonState == 1) {
    chanal1 = 20;
  }
}
BLYNK_WRITE(V6) {
  buttonState = param.asInt();
  if (buttonState == 1) {
    chanal2 = 6;
  }
}
BLYNK_WRITE(V7) {
  buttonState = param.asInt();
  if (buttonState == 1) {
    chanal3 = 6;
  }
}
BLYNK_WRITE(V8) {
  buttonState = param.asInt();
  if (buttonState == 1) {
    chanal4 = 20;
  }
}
void loop() {

  Blynk.run();
  if (digitalRead(bt1) == HIGH) {
    if (digitalRead(bt1) == HIGH) {
      chanal1 -= 1;
      if (chanal1 < 0) {
        chanal1 = 0;
      }
      Blynk.email("@email.nu.ac.th", "TABLET 1 Onclick", chanal1);
      Serial.println("BT1 : ON");
      Serial.println(chanal1);
    }
  }
  if (digitalRead(bt2) == HIGH) {
    if (digitalRead(bt2) == HIGH) {
      chanal2 -= 1;
      if (chanal2 < 0) {
        chanal2 = 0;
      }
      Blynk.email("@email.nu.ac.th", "TABLET 2 Onclick", chanal2);
      Serial.println("BT2 : ON");
      Serial.println(chanal2);
    }
  }
  if (digitalRead(bt3) == HIGH) {
    if (digitalRead(bt3) == HIGH) {
      chanal3 -= 1;
      if (chanal3 < 0) {
        chanal3 = 0;
      }
      Blynk.email("@email.nu.ac.th", "TABLET 3 Onclick", chanal3);
      Serial.println("BT3 : ON");
      Serial.println(chanal3);
    }
  }
  if (digitalRead(bt4) == HIGH) {
    if (digitalRead(bt4) == HIGH) {
      chanal4 -= 1;
      if (chanal4 < 0) {
        chanal4 = 0;
      }
      Blynk.email("@email.nu.ac.th", "TABLET 4 Onclick", chanal4);
      Serial.println("BT3 : ON");
      Serial.println(chanal4);
    }
  }
  if (digitalRead(bt5) == HIGH) {
    if (digitalRead(bt5) == HIGH) {
      chanal1 -= 1;
      chanal2 -= 1;
      chanal3 -= 1;
      if (chanal1 < 0) {
        chanal1 = 0;
      }
      if (chanal2 < 0) {
        chanal2 = 0;
      }
      if (chanal3 < 0) {
        chanal3 = 0;
      }
      Blynk.email("@email.nu.ac.th", "MORNING Onclick", "MORNING Onclick");
      Serial.println("MORNING : ON");
    }
  }
  if (digitalRead(bt6) == HIGH) {
    if (digitalRead(bt6) == HIGH) {
      chanal2 -= 1;
      chanal3 -= 1;
      chanal4 -= 1;
      if (chanal2 < 0) {
        chanal2 = 0;
      }
      if (chanal3 < 0) {
        chanal3 = 0;
      }
      if (chanal4 < 0) {
        chanal4 = 0;
      }
      Blynk.email("@email.nu.ac.th", "AFTERNOON Onclick", "AFTERNOON Onclick");
      Serial.println("AFTERNOON : ON");
    }
  }
  if (digitalRead(bt7) == HIGH) {
    if (digitalRead(bt7) == HIGH) {
      chanal1 -= 1;
      chanal2 -= 1;
      chanal3 -= 1;
      chanal4 -= 1;
      if (chanal1 < 0) {
        chanal1 = 0;
      }
      if (chanal2 < 0) {
        chanal2 = 0;
      }
      if (chanal3 < 0) {
        chanal3 = 0;
      }
      if (chanal4 < 0) {
        chanal4 = 0;
      }
      Blynk.email("@email.nu.ac.th", "EVENNING Onclick", "EVENNING Onclick");
      Serial.println("EVENNING : ON");
    }
  }

  Blynk.virtualWrite(1, chanal1);
  Blynk.virtualWrite(2, chanal2);
  Blynk.virtualWrite(3, chanal3);
  Blynk.virtualWrite(4, chanal4);

}
