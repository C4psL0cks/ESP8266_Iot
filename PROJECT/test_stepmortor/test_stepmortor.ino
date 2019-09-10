int in1 = 5;
int in2 = 6;
int sw = 9;
void setup() {

  Serial.begin(115200);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
}

void loop(void) {
  
  if (digitalRead(sw) == LOW) {
    foward();
    delay(500);
    black();
    delay(500);
    stop();
  }
}
void foward() {
  analogWrite(in1, 0);
  analogWrite(in2, 400);
}
void black() {
  analogWrite(in1, 400);
  analogWrite(in2, 0);
}
void stop() {
  analogWrite(in1, 0);
  analogWrite(in2, 0);
}
