#define PIN_D1 5  // gpio5 = D1  PWM_A
#define PIN_D2 4  // gpio4 = D2  PWM_B
#define PIN_D3 0  // gpio0 = D3  DA (A- A+) подключается двигатель (светодиоды)
#define PIN_D4 2  // gpio2 = D4  DB (B- B+) подключается двигатель

void setup() 
   {
    pinMode(PIN_D1, OUTPUT); // инициализируем Pin как выход
    pinMode(PIN_D2, OUTPUT); // инициализируем Pin как выход
    pinMode(PIN_D3, OUTPUT); // инициализируем Pin как выход
    pinMode(PIN_D4, OUTPUT); // инициализируем Pin как выход
   }

void loop()
{
  digitalWrite(PIN_D1, HIGH);   // PWM_A HIGH
  digitalWrite(PIN_D3, HIGH);   // DA HIGH
  delay(2000);                  // wait 
  digitalWrite(PIN_D3, LOW);    // DA LOW
  delay(2000);                  // wait

  for(int i=0; i<=255; i++)     // изменяется яркость светодиодов на контактах (A- A+)
    {
    analogWrite(PIN_D1,i);
    delay(10);
    }
  for(int i=255; i>=0; i--)
    {
    analogWrite(PIN_D1,i);
    delay(10);
    }

    digitalWrite(PIN_D2, HIGH);   // PWM_B HIGH, изменяется направление вращения двигателя на контактах (B- B+)
    digitalWrite(PIN_D4, HIGH);   // DB HIGH
    delay(2000);                  // wait 
    digitalWrite(PIN_D4, LOW);    // DB LOW
    delay(2000);                  // wait 
    digitalWrite(PIN_D2, LOW);    // PWM_B LOW

 }
