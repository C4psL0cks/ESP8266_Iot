
int relay1 = 7;
int relay2 = 8;

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

 
}
