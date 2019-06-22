/// led 1 and 2
#define led1 33
#define led2 32

/// motor 1
#define in1  14
#define in2  27
#define en1   2

/// motor 2
#define in3  26
#define in4  25
#define en2  13

/// switch 2 channal
#define sw1  34
#define sw2  35

/// buzzer
#define buzzer 12

/// sensor 5 channal 3.3 v 1 channal
#define sensor1 15  // 3.3 v pull-up
#define sensor2 5   // 5 v pull-up
#define sensor3 36  // 5 v pull-up
#define sensor4 39  // 5 v pull-up

/// SPI mode
#define SCK  18
#define MOSI 23
#define SS   16
#define MISO 19

/// I2C mode
#define SCL  22
#define SDA  21


void testswitch() {

  if (digitalRead(sw1) == LOW) {
    Serial.println(digitalRead(sw1));
  }
  if (digitalRead(sw2) == LOW) {
    Serial.println(digitalRead(sw2));
  }
}
void testmotor() {
  if (digitalRead(sw1) == LOW)
  {
    forward();
    digitalWrite(led1, LOW);
  }
  else if (digitalRead(sw2) == LOW)
  {
    reverse();
    digitalWrite(led2, LOW);
  } else
  {
    stop();
    digitalWrite(led2, HIGH);
    digitalWrite(led1, HIGH);
  }
}

void testsensor() {

}
/// เดินหน้า
void forward()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(en1, HIGH);
}
/// ถอยหลัง
void reverse()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(en1, HIGH);
}
/// หยุด
void stop()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(en1, LOW);
}

void testsensor() {

  if (digitalRead(sensor1) == LOW) {
    Serial.println(digitalRead(sensor1));
  }
  if (digitalRead(sensor2) == LOW) {
    Serial.println(digitalRead(sensor2));
  }
  if (digitalRead(sensor3) == LOW) {
    Serial.println(digitalRead(sensor3));
  }
  if (digitalRead(sensor4) == LOW) {
    Serial.println(digitalRead(sensor4));
  }
}
void setup()
{
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // moter
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(en1, OUTPUT);

  // switch
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);

  // sensor
  pinMode(sensor1 , INPUT);
  pinMode(sensor2 , INPUT);
  pinMode(sensor3 , INPUT);
  pinMode(sensor4 , INPUT);
}
void loop() {

  testswitch();
  testmotor();
  testsensor();
}
