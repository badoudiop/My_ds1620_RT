#include <Arduino.h>
#include <TM1637Display.h>
#include "My_ds1620_RT.h"

// Module connection pins (Digital Pins)
#define CLK 6
#define DIO 5

// The amount of time (in milliseconds) between tests
#define TEST_DELAY   2000

TM1637Display display(CLK, DIO);
int y,t , temp1,temp2,dizaine,diz , unit , decimale,deci1,deci2,mama;// ne pas utiliser uint8_t , ça ne marche pas .

//**********************************************
void setup()
{
  y=0;t=0;
Serial.begin(9600);
pinMode(spi_clk,OUTPUT);
pinMode(spi_reset,OUTPUT);

	start();
	SPI_init();
display.setBrightness(0x0f);
	pause(200);
 }
 //********************************************
void loop()
{
uint8_t data[]={1,2,3,4};//ne pas utiliser int data[]= ... ça ne marche pas .
	//charger();
     t=(y&0x01)*5;
//*********************************************
mama=realtemp()*100;
decimale=t;
dizaine=mama/100;
	  diz=dizaine/10;
unit=dizaine%10;
decimale=mama%100;
deci1=decimale/10;
deci2=decimale%10;
//******************
  data[0] = display.encodeDigit(diz);// uint8_t data[]={diz,unit,0b1100011,t)};ne marche pas , les chiffres sont mal formé ; 
  data[1] =  display.encodeDigit(unit);
  data[2] = 0b1100011;//pour les degrés ne marche pas avec encodeDigit car on obtient un nombre au lieu du symbole degré.
  data[3] = 0b00;//display.encodeDigit(t);
  display.setSegments(data);
display.setBrightness(0x0f);
  delay(TEST_DELAY);
data[0]=display.encodeDigit(deci1);
data[1]=display.encodeDigit(deci2);
data[2]=0b00000000;//display.encodeDigit(diz);
data[3]=0b00000000;//display.encodeDigit(diz);
display.setSegments(data);

display.setBrightness(0x0f);
  delay(TEST_DELAY);

//***********************************************
 
   Serial.print("temperature : ");
	  Serial.println(realtemp());
    /*
   Serial.print("mama");
   Serial.println(mama);
    _delay_ms(500);
    */
}
//*****************************************************
void do_haut(void)
{
pinMode(spi_do,OUTPUT);
digitalWrite(spi_do,HIGH);
pause(5);
}
//**********************************************	
void do_bas(void)
{
pinMode(spi_do,OUTPUT);
digitalWrite(spi_do,LOW);
pause(5);
}
//*************************************************
void clock(void)
{
digitalWrite(spi_clk,LOW);
pause(5);
digitalWrite(spi_clk,HIGH);
pause(5);
}
//****************************************************
void start(void){
digitalWrite(spi_reset,HIGH);
pause(5);
//****************
}
//************************************************
	void stop(void)
	{
digitalWrite(spi_reset,LOW);
pause(5);
	}
//************************************************
void SPI_init	(void)
{
	start();
//;config command protocole 	pour ecrire au config
	putbyte(0x0C);
//;output polarity active hight + conversion non CONTINUE : one shot 
	putbyte(0x03);
	stop();
}
//*******************************************
void pause(unsigned int j)
{
while(j--) _delay_us(1);
}
//*******************************************
void charger(void)
{
deb_convert();
	mesure();
	fin_convert();
	}
//*******************************************
void deb_convert(void)
{

	start();
	putbyte(0xEE);//0xEE=debut conversion
	pause(200);
	stop();
	}
//****************************************
void fin_convert(void)
{	
	start();
putbyte(0x22);//0x22 pour fin conversion
	pause(200);
stop();
}
//************************************************
void mesure(void)
{	
uint8_t z=0;
	start();
	putbyte(0xAA);// 0xAA pour mesurer
  _delay_ms(750);
	y= getbyte();
 z=getbyte();
	stop();	
	}
//******************************************
void putbyte (unsigned char x)
{
if(x&0x01) do_haut(); 
else do_bas();
clock();

if(x&0x02) do_haut(); 
else do_bas();
clock();

if(x&0x04) do_haut(); 
else do_bas();
clock();

if(x&0x08) do_haut(); 
else do_bas();
clock();

if(x&0x10) do_haut(); 
else do_bas();
clock();

if(x&0x20) do_haut(); 
else do_bas();
clock();

if(x&0x40) do_haut(); 
else do_bas();
clock();

if(x&0x80) do_haut(); 
else do_bas();
clock();
}
//*******************************
unsigned int getbyte(void)
{
unsigned int x=0;
pinMode(spi_do,INPUT);
clock();
for(int i=0;i<8;i++){if((digitalRead(spi_do))&0x01) x|=(1<<i);clock();}
return x;
}
//*************************************
uint8_t read_slope()//=counter_per_c
{
  uint8_t counter=0,counter1=0;
start();
putbyte(0xA9);
counter=getbyte();//byte LSB
counter1=getbyte();// byte MSB 
stop();
return counter1*16+counter;
}
//*****************************************
uint8_t read_counter()//=count_remain
{
  uint8_t counter=0,counter1=0;
start();
putbyte(0xA0);
counter=getbyte();// byte LSB
counter1=getbyte();// byte MSB
stop();
return counter1*16+counter;
}

//**********************************************
float realtemp()
{
  float latemp=0;
charger();
y=y/2;
latemp=y-0.25+((float)read_slope()-(float)read_counter())/(float)read_slope();
return latemp-1;// sans /2 on a le double .
}
