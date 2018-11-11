ADC_MODE(ADC_VCC);
void setup() {
  Serial.begin(115200);
}

void loop() {
  // Do something
  int sensorValue = ESP.getVcc();
  float volts = (float)sensorValue / 1023.00;
  Serial.println(volts);
  delay(100);
}

//ADC_MODE(ADC_VCC); // 3.3v voltage sensor
//int vcc;
//
//void setup() {
//  Serial.begin(115200);
//}
//
//void loop() {
//  vcc = ESP.getVcc();
//  double dvcc = (float)vcc / 1024;
//  Serial.println("Voltage: " + String(dvcc, 3) + "V");
//  delay(500);
//}
