int pindigital3 = 3;
int pindigital4 = 4;
int pindigital5 = 5;
int pindigital6 = 6;
int pindigital7 = 7;
int pindigital8 = 8;
int pindigital9 = 9;
int pindigital10 = 10;
long int number;
void setup() {

  Serial.begin(115200);
  while (!Serial) {}
  pinMode(pindigital3, OUTPUT);
  pinMode(pindigital4, OUTPUT);
  pinMode(pindigital5, OUTPUT);
  pinMode(pindigital6, OUTPUT);
  pinMode(pindigital7, OUTPUT);
  pinMode(pindigital8, OUTPUT);
  pinMode(pindigital9, OUTPUT);
  pinMode(pindigital10, OUTPUT);
  digitalWrite(pindigital3, HIGH);
  digitalWrite(pindigital4, HIGH);
  digitalWrite(pindigital5, HIGH);
  digitalWrite(pindigital6, HIGH);
  digitalWrite(pindigital7, LOW);
  digitalWrite(pindigital8, LOW);
  digitalWrite(pindigital9, LOW);
  digitalWrite(pindigital10, LOW);
  //Serial.println("Start Read....");

}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    // read the incoming byte:
    number = Serial.parseInt();
    //Serial.println(number);
    switch (number) {
      /// 4 ตัวสุดแรก active high
      case 10:
        digitalWrite(pindigital3, LOW);
        //Serial.println("PIN 3 LOW");
        break;
      case 11:
        digitalWrite(pindigital3, HIGH);
        //Serial.println("PIN 3 HIGH");
        break;
      case 20:
        digitalWrite(pindigital4, LOW);
        //Serial.println("PIN 4 LOW");
        break;
      case 21:
        digitalWrite(pindigital4, HIGH);
        //Serial.println("PIN 4 HIGH");
        break;
      case 30:
        digitalWrite(pindigital5, LOW);
        //Serial.println("PIN 5 LOW");
        break;
      case 31:
        digitalWrite(pindigital5, HIGH);
        //Serial.println("PIN 5 HIGH");
        break;
      case 40:
        digitalWrite(pindigital6, LOW);
        //Serial.println("PIN 6 LOW");
        break;
      case 41:
        digitalWrite(pindigital6, HIGH);
        //Serial.println("PIN 6 HIGH");
        break;
      /// 4 ตัวสุดท้าย active low
      case 50:
        digitalWrite(pindigital7, HIGH);
        //Serial.println("PIN 7 LOW");
        break;
      case 51:
        digitalWrite(pindigital7, LOW);
        //Serial.println("PIN 7 HIGH");
        break;
      case 60:
        digitalWrite(pindigital8, HIGH);
        //Serial.println("PIN 8 LOW");
        break;
      case 61:
        digitalWrite(pindigital8, LOW);
        //Serial.println("PIN 8 HIGH");
        break;
      case 70:
        digitalWrite(pindigital9, HIGH);
        //Serial.println("PIN 9 LOW");
        break;
      case 71:
        digitalWrite(pindigital9, LOW);
        //Serial.println("PIN 9 HIGH");
        break;
      case 80:
        digitalWrite(pindigital10, HIGH);
        //Serial.println("PIN 10 LOW");
        break;
      case 81:
        digitalWrite(pindigital10, LOW);
        //Serial.println("PIN 10 HIGH");
        break;
    }
  }
}
