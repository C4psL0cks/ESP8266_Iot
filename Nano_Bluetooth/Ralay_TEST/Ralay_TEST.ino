
int relay1 = 6;
int relay2 = 7;

void setup() {
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, HIGH);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, HIGH);

}

void loop() {
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  delay(1000);
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  delay(1000);

}
