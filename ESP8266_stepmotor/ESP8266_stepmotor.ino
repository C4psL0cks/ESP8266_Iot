
char check;
int state;
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
}

void loop()
{

  check = Serial.read();
  if (Serial.available() > 0)
  {
    //Serial.println(check);
    if (check == '1')
    {
      Serial.println(check);
      state = 1;
    }
    else if (check == '2')
    {
      Serial.println(check);
      state = 2;
    }
  }
  if (state == 1)
  {
    Forword();
  }
  if (state == 2)
  {
    back();
  }
}

void Forword()
{
  analogWrite(D1, 0);
  analogWrite(D2, 400);
  delay(1000);
  analogWrite(D1, 400);
  analogWrite(D2, 0);
  delay(1000);
}
void back()
{

  analogWrite(D1, 400);
  analogWrite(D2, 0);
  delay(1000);
  analogWrite(D1, 0);
  analogWrite(D2, 400);
  delay(1000);
}
