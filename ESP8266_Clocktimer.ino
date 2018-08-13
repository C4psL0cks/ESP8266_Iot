boolean nn;
int A = A0;
int m = 00, m1 = 0;
int s = 0, s1 = 0;
int ms = 0;

void setup ()
{
  Serial.begin(9600);
  pinMode (A, INPUT);
  Serial.println("Start");
}
void loop()
{

  /*************STSRT*************/
  nn = digitalRead(A);
  if (nn == 1)
  {
    for (ms = 0; ms <= 60; ms++)
    {
      if (ms == 60) {
        m++;
      }
      if (m == 10)
      {
        m1++;
        m = 0;
      }
      delay(20);
      Serial.println();
      Serial.print(s1);
      Serial.print(s);
      Serial.print(":");
      Serial.print(m1);
      Serial.print(m);
      Serial.print(":");
      Serial.print(ms);
    }
    if (m1 == 6)
    {
      s++;
      m = 0;
      m1 = 0;
    }
  }

  /***************** STOP AND RESET**********************/
  nn = digitalRead(A);
  if (nn == 0)
  {
    Serial.print("break");
    m = 0;
    m1 = 0;
    s = 0;
    s1 = 0;
    ms = 0;
    Serial.println("Reset");
    delay(1000);
    Serial.println("Timer");
    //Serial.println("00:00:00");
  }
}
