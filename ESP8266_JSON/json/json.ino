void setup() {
  Serial.begin(9600);
  String name = "supot";
  int age = 56;

  Serial.println("{\"name\": \"" + name + "\", \"age\":" + String(age) + "}");

}

void loop() {
  // put your main code here, to run repeatedly:

}
