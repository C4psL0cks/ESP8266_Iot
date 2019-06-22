การใช้งานเริ่มต้น ESP8266 NodeMCU และการใช้งาน Application ต่างๆ
---------------------------------------------
- แหล่งรวบรวม iot และเผยแพร่ สำหรับผู้สนใจนำไปต่อยอด โดยสามารถใช้ได้กับ ESP8266 และ ESP32    
- การใช้งาน LINE API 
- การใช่งาน SQL 
- การใช้งาน Firebase 
- การใช้งาน blynk
- การใช้งาน sensoer DHT22,BME820
- การใช้งาน ic ขยายขา digital

โปรเจคต่างๆ ที่ใช้ ESP8266 NodeMCU และการใช้งาน Application ต่างๆ
---------------------------------------------
- Mark ระบบ smart lock alert Startup
- Rice ระบบวัดระดับน่ำในนาข้าว บนแอปพลิเคชัน
- SmartCantus ระบบความคุมการเลี้ยงต้น cantus
- Weighttacking ระบบชั่งน้ำหนักและติดตามผล บนแอปพลิเคชัน

วิธีสร้าง Line Token
---------------------------------------------
ทำการ Log in ที่ https://notify-bot.line.me/en/

จากนั้น เข้าไปที่ https://notify-bot.line.me/my/  เพื่อทำการ สร้าง Line Token ขึ้นมา
ให้กดปุ่ม **Generate token**

ตั้งชื่อ Token อะไรก็ได้
พร้อมทั้งเลือก กลุ่มใน Line ที่ต้องการให้มีการแจ้งเตือนไปยังกลุ่ม

วิธีสร้าง Wifi manager ร่วมกับ blynk
---------------------------------------------
เข้าไปใน Exsample wifi manager 
ให้เพิ่มคำสั่ง blynk.config(token); เข้าไป ก็จะสามารถเชื่อมต่อกับ blynk ได้

วิธีเพิ่มขา Digital
---------------------------------------------
ขยายพอร์ตดิจิตอล I/O ด้วย MCP23017
ขยาย Analog กับ ขยาย Digital.
- Analog ใช้ 4051
- Digital ใช้ 74HC595
- Analog ใช้ MCP3008
- IC Boost Regulator -->1.8-5.5 โวล สูงสุด 1A 

Rf.
- https://arduino-esp8266.readthedocs.io/en/latest/index.html
- https://github.com/esp8266
- https://firebase-arduino.readthedocs.io/en/latest/
- https://www.praphas.com/forum/index.php?PHPSESSID=7b9fa1rmfb5eit80kmtkkto662&topic=327.0
- https://www.praphas.com/forum/index.php?board=27.0

Version
=========
- Mark  ระบบต้นแบบ
- Mark I ตัวสตาดอัพอับ v2
- Mark II ตัวโปรเจคจบ v1
- Mark III ตัวโปรเจคจบ v1_2


