#define MOTOR_A_PIN A0 //ขาที่  A0 
#define MOTOR_B_PIN A1 //ขาที่  A1 

void MotorSetup() {
  pinMode(MOTOR_A_PIN, OUTPUT); //สั่งให้เป็น OUTPUT
  pinMode(MOTOR_B_PIN, OUTPUT); //สั่งให้เป็น OUTPUT
}

void MotorSet(int stats) {
  if (stats == MOTER_FORWARD) { //หมุนตาม
    digitalWrite(MOTOR_A_PIN, 0);
    digitalWrite(MOTOR_B_PIN, 1);
  } else if (stats == MOTER_BACKWARD) { //หมุนทวน
    digitalWrite(MOTOR_A_PIN, 1);
    digitalWrite(MOTOR_B_PIN, 1);
  } else { //Motor Stop
    digitalWrite(MOTOR_A_PIN, 0);
    digitalWrite(MOTOR_B_PIN, 0);
  }
}

