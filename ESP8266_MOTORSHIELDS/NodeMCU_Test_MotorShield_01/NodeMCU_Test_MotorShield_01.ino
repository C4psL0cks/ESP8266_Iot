//   -- NodeMcu_Test_MotorShield_01 -- 
  
// определение режима соединения и подключение библиотеки RemoteXY  
#define REMOTEXY_MODE__ESP8266WIFI_LIB_POINT
#include <ESP8266WiFi.h> 
#include <RemoteXY.h> 

// настройки соединения  
#define REMOTEXY_WIFI_SSID "RemoteXY" 
#define REMOTEXY_WIFI_PASSWORD "" 
#define REMOTEXY_SERVER_PORT 6377 

// конфигурация интерфейса   
#pragma pack(push, 1) 
uint8_t RemoteXY_CONF[] = 
  { 255,2,0,0,0,42,0,6,5,0,
  1,0,13,10,21,21,2,49,0,4,
  128,15,44,71,10,2,130,1,8,6,
  85,52,13,129,0,47,15,28,11,9,
  208,162,208,149,208,161,208,162,0 }; 
   
// структура определяет все переменные вашего интерфейса управления  
struct { 

    // input variable
  uint8_t button_1; // =1 если кнопка нажата, иначе =0 
  int8_t slider_1; // =0..100 положение слайдера 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY; 
#pragma pack(pop) 

///////////////////////////////////////////// 
//           END RemoteXY include          // 
///////////////////////////////////////////// 

#define PIN_BUTTON_1 4
#define PIN_2 5

void setup()  
{ 
  RemoteXY_Init ();  
   
  pinMode (PIN_BUTTON_1, OUTPUT);
  pinMode (PIN_2, OUTPUT);
  RemoteXY.slider_1 = 0;
   
} 

void loop()  
{  
  RemoteXY_Handler (); 
  digitalWrite(PIN_BUTTON_1, (RemoteXY.button_1==0)?LOW:HIGH);
  analogWrite(PIN_2, RemoteXY.slider_1 * 2.55); 
  delay(10);

}
