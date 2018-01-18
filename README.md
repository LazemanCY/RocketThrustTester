# Rocket Thrust Tester
It's a thrust measure tool for amateur rocket solid engine.<br />
H711 and deformation force sensor are used for measurement.<br />
MCU is ATmega328p and code under ArduinoIDE<br />
This version is made for small rocket whose max-thrust is less than 3kgf and works within 2 seconds. <br />

## 12864LCD
My LCD (ST7920 chip) is set to serial command mode. U8g2 lib is used to provide display function.<br />
https://github.com/olikraus/u8g2/wiki/setup_tutorial#u8g2-full-buffer--page-buffer-and-u8x8-mode 

## HX711 and Weight Sensor
HX711 should be set to high speed sample mode: RATE pin is soldered to VDD.<br />
The weight sensor I used has a measure range of 3kg.<br />
![](https://github.com/LazemanCY/rocket-thrust-tester/blob/master/HX711.jpg)

## Arduino
Nano plays the whole thing which is a bit overwhelming for its tiny body.<br />
LCD library tokes up most of the RAM and about 300 Bytes are left for other features.<br />

## Wiring
All of them are powered by ArduinoNano's onboard rectifier. A 7.4V Li-Po is connected to Vin.<br />
I made a box for the lcd and Arduino and mounted it on a board.<br />
![](https://github.com/LazemanCY/rocket-thrust-tester/blob/master/Wiring.jpg)
![](https://github.com/LazemanCY/rocket-thrust-tester/blob/master/Appearance.jpg)

## How to Use
Prepare everything and then power-up, Arduino starts to get the zero point. <br />
After 3 seconds, lcd will show "Ready to Go!" which means a thrust bigger than 10g will trigger the system to collect data.<br />
It's sampling rate is 50Hz and a total 100 data points will be recorded in 2 seconds.<br />
When the measurement is finished, you will see a thrust curve. Push the button could jump to statistical data. <br />
Last two pages show the raw data for looking over.<br />
For ATmega328p limited storage space, a mega or serial port to PC could be used for longer record time.<br />
![](https://github.com/LazemanCY/rocket-thrust-tester/blob/master/Ready.jpg)
![](https://github.com/LazemanCY/rocket-thrust-tester/blob/master/18-1-18%201025%20test.jpg)