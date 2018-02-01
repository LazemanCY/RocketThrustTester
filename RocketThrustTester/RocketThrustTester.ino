#include <Arduino.h>
#include <U8g2lib.h>
#include "HX711.h"

//FONT
#define FONT_BIG    u8g2_font_ncenB08_tf
#define FONT_MED    u8g2_font_profont12_mf 
#define FONT_SMALL  u8g2_font_4x6_mf 

//Drawing
#define zero_point_x 10
#define zero_point_y 57

#ifdef U8X8_HAVE_HW_SPI 
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//full page buffer mode, which is ram consuming
U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, /* CS=*/ 10, /* reset=*/ 8);

HX711 HX711_CH0(2, 3, 670); //SCK,DT,GapValue
//HX711's SCK connect to D2
//HX711's DT connect to D3
//GapValue is used to trim weight reading value

#define KEY  4

unsigned long previous_millis_sample = 0;

uint8_t sample_cycle = 20;
uint8_t sample_rate = 50;
uint8_t time_scale = 2;

uint16_t Thrust[100];//0.01N
uint32_t impulse = 0;
uint16_t maxForce = 0;
uint16_t avgForce;
uint16_t maxForceTime = 0;
uint16_t burnTime = 0;
long forceRead = 0;		//unit in g

//add decimal point '.' to a u16 num in 1/100 scale
//eg: 3365 will be 33.65
const char *u16toftp(char * dest, uint16_t v)
{
  uint8_t pos;
  uint8_t d;
  uint16_t c;
  c = 10000;
  for( pos = 0; pos < 5; pos++ )
  {
      d = '0';
      while( v >= c )
      {
        v -= c;
        d++;
      }
      dest[pos] = d;
      c /= 10;
  }  
  dest[6] = '\0';
  dest[5] = dest[4];
  dest[4] = dest[3];
  dest[3] = '.';
  return dest;
}
/* v = value, d = number of digits */
const char *u16toft(uint16_t v, uint8_t d)
{
  static char buf[7];
  d = 5-d;
  return u16toftp(buf, v) + d;
}

void setup()
{
  u8g2.begin();  
  
  pinMode(KEY, INPUT);
  digitalWrite(KEY, INPUT_PULLUP);
  delay(100);
  
  //Serial.begin(9600);//disable serial could save some ram
  
  u8g2.clearBuffer();
  u8g2.setFont(FONT_BIG);
  u8g2.drawStr(7,32,"Rocket Engine Tester");
  
  //power up with key down will enter slow sample mode for long working time engine.
  //20Hz sample rate for 5 seconds.
  if(digitalRead(KEY) == LOW)
  {
    u8g2.drawStr(7,52,"in Low Sample Rate");
    //over write sample params
    sample_cycle = 50;
    sample_rate = 20;
    time_scale = 5;
  }
  u8g2.sendBuffer();

  HX711_CH0.begin();					//get zero point
  delay(3000);								//delay 3s untill stable
  HX711_CH0.begin();					//get zero point one more time
  
  u8g2.clearBuffer();
  u8g2.setFont(FONT_BIG);
  u8g2.drawStr(17,32,"Ready to Go!");
  u8g2.sendBuffer();
}

void loop()
{   
  uint8_t i = 0, j = 0;
  uint8_t count = 0;
  uint8_t screen = 0;
  uint8_t x ,y;
  uint8_t force_pixel;
  uint32_t temp_32b;
  
  unsigned long currentMillis;
  
  //wait until thrust is larger than 10g
  do{
    forceRead = HX711_CH0.Get_Weight();
    delay(25);
  }while( forceRead < 10 );//too small will make the system irritability, it could be larger
  
  previous_millis_sample = millis();//it's important to give it a init value
  //get 100 samples in 2 seconds
  while(i < 100)
  {
    currentMillis = millis();
    if((currentMillis - previous_millis_sample) >= sample_cycle) 
    {
      previous_millis_sample += sample_cycle;
      forceRead = HX711_CH0.Get_Weight();
      if(forceRead < 0) forceRead = 0;
      Thrust[i] = (uint16_t)(forceRead * 98 / 100);//convert g to N
      if(Thrust[i] > maxForce){
        maxForce = Thrust[i];
        maxForceTime = i;//in 0.02s step
      }
      //accumulate burn time for thrust > 5
      if(forceRead > 5)
        burnTime++;
      
      impulse += Thrust[i];
      i++;
    }
  }
    
  avgForce = impulse/burnTime; //get average force before normalizing
  
  impulse /= sample_rate;//50 samples in one second
  maxForceTime *= time_scale;//ticks every 0.02s, 10th tick is 0.2s
  burnTime *= time_scale;
  
  /*
  for(i=0;i<100;i++)
  {
    Serial.println(Thrust[i]);
    delay(10);
  }
  //*/
  
  //After one measurement, go to screen loop and never jump out.
  //if a new measurement is desired, just push the reset button... 
  while(1){
    //----------Thrust Curve----------
    //The first page shows the force/time curve
    u8g2.clearBuffer();
    u8g2.setFont(FONT_SMALL);
    u8g2.drawStr(2,6,"f");
    u8g2.drawStr(120,62,"t");
    
    u8g2.drawLine(zero_point_x, zero_point_y, zero_point_x, zero_point_y - 55); //y axis
    u8g2.drawLine(zero_point_x - 3, zero_point_y - 25, zero_point_x, zero_point_y - 25); 
    u8g2.drawLine(zero_point_x - 3, zero_point_y - 50, zero_point_x, zero_point_y - 50); 

    
    u8g2.drawLine(zero_point_x, zero_point_y, zero_point_x + 105, zero_point_y);//x axis
    if(sample_rate == 50)
    {
      u8g2.drawLine(zero_point_x + 50, zero_point_y, zero_point_x + 50, zero_point_y + 3);
      u8g2.drawLine(zero_point_x + 100, zero_point_y, zero_point_x + 100, zero_point_y + 3);
    }
    else
    {
      u8g2.drawLine(zero_point_x + 20, zero_point_y, zero_point_x + 20, zero_point_y + 3);
      u8g2.drawLine(zero_point_x + 40, zero_point_y, zero_point_x + 40, zero_point_y + 3);
      u8g2.drawLine(zero_point_x + 60, zero_point_y, zero_point_x + 60, zero_point_y + 3);
      u8g2.drawLine(zero_point_x + 80, zero_point_y, zero_point_x + 80, zero_point_y + 3);
      u8g2.drawLine(zero_point_x + 100, zero_point_y, zero_point_x + 100, zero_point_y + 3);
    }
    
    //draw the cruve
    for(x=0; x < 100; x++)
    {
      temp_32b = (uint32_t)Thrust[x] * 5 / 294;
      force_pixel = (uint8_t)temp_32b;
      if(force_pixel > 50) force_pixel = 50;
      u8g2.drawPixel(zero_point_x + x,zero_point_y - force_pixel); 
    }
    u8g2.sendBuffer();
    delay(500);
    
    while(digitalRead(KEY) == HIGH) delay(5);//a simple loop to wait for key
    
    //----------Statistic Data----------
    u8g2.clearBuffer();
    u8g2.setFont(FONT_MED);
    
    u8g2.drawStr(0,10," Impulse:");
    u8g2.drawStr(60,10,u16toft((uint16_t)impulse,5));
    u8g2.drawStr(100,10,"Ns");
    
    u8g2.drawStr(0,22,"  avgF  :");//average thrust
    u8g2.drawStr(60,22,u16toft(avgForce,5));
    u8g2.drawStr(100,22,"N");
    
    u8g2.drawStr(0,34,"  maxF  :");//max thrust
    u8g2.drawStr(60,34,u16toft(maxForce,5));
    u8g2.drawStr(100,34,"N");
    
    u8g2.drawStr(0,46,"  maxF T:");//time to reach max thrust
    u8g2.drawStr(70,46,u16toft((uint16_t)maxForceTime,3));
    u8g2.drawStr(100,46,"s");
    
    u8g2.drawStr(0,58,"  burn T:");//burning time
    u8g2.drawStr(70,58,u16toft((uint16_t)burnTime,3));
    u8g2.drawStr(100,58,"s");
    
    u8g2.sendBuffer();
    delay(500);
    
    while(digitalRead(KEY) == HIGH) delay(5);
    
    //----------RAW DATA 1----------
    u8g2.clearBuffer();
    u8g2.setFont(FONT_SMALL);
    for(i=0;i<5;i++)
    {
      for(j=0;j<10;j++) 
      {
        u8g2.drawStr(i*25+6,6+j*6,u8g2_u16toa(Thrust[i*10+j],4));
      }      
    }
    u8g2.sendBuffer();
    delay(500);
    
    while(digitalRead(KEY) == HIGH) delay(5);
    
    //----------RAW DATA 2----------
    u8g2.clearBuffer();
    u8g2.setFont(FONT_SMALL);
    for(i=0;i<5;i++)
    {
      for(j=0;j<10;j++) 
      {
        u8g2.drawStr(i*25+6,6+j*6,u8g2_u16toa(Thrust[i*10+j+50],4));
      }      
    }
    u8g2.sendBuffer();
    delay(500);
    
    while(digitalRead(KEY) == HIGH) delay(5);
  }
}
