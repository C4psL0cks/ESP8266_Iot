///////// Motor activation and stopping it my switch or time ///////////
unsigned long previousMillisMotor = 0;
int intervalMotor = 10000;  // maximum working time
const byte  buttonPin = 2;
const byte  motorPin = 5;
const byte  stopingPin = 12;
int buttonState = 0;
int buttonStopState = 0;

void setup() {

  pinMode(buttonPin, INPUT_PULLUP );
  pinMode(stopingPin, INPUT_PULLUP );
  pinMode(motorPin, OUTPUT );
  Serial.begin(9600);

}

void loop() {

  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    motorrun();
  }
  else {
    digitalWrite(motorPin, LOW);
    Serial.println(buttonState);
  }
}

void motorrun() {

  unsigned long currentMillis = millis();
  buttonStopState = digitalRead(stopingPin);
  digitalWrite(motorPin, HIGH);
  Serial.println(buttonState);
  if (buttonStopState == LOW) {
    digitalWrite(motorPin, LOW);
  }
  while (buttonStopState == HIGH) {
    if ((unsigned long)(currentMillis - previousMillisMotor) >= intervalMotor) {
      digitalWrite(motorPin, LOW);
      Serial.println(" time limit ");

      previousMillisMotor = currentMillis;    // save current time to motor previousMillis

    }
  }
}
