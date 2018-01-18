# Rocket Thrust Tester
It's a thrust measure tool for amateur rocket solid engine.<br />
H711 and deformation force sensor are used for measurement.<br />
MCU is ATmega328p and code under ArduinoIDE<br />
This version is made for small rocket whose max-thrust is less than 3kgf and works within 2 seconds. <br />

##12864LCD
My LCD (ST7920 chip) is set to serial command mode. And u8g2 lib is used to provide display function.<br />
https://github.com/olikraus/u8g2/wiki/setup_tutorial#u8g2-full-buffer--page-buffer-and-u8x8-mode 

##HX711 and Weight Sensor
HX711 should be set to high speed sample mode: RATE pin is soldered to VDD.<br />
The weight sensor I used has a measure range of 3kg.<br />

##Arduino
Nano plays the whole thing which is a bit overwhelming for its tiny body.<br />
LCD library tokes up most of the RAM and about 300 Bytes are left for other features.<br />

##How to Use