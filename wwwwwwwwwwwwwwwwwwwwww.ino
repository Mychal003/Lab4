#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#include "LCDI2C.h"

#ifndef _BV
#define _BV(bit)        (1<<(bit))
#endif
#ifndef sbi
#define sbi(reg,bit)    reg |= (_BV(bit))
#endif

#ifndef cbi
#define cbi(reg,bit)    reg &= ~(_BV(bit))
#endif

#ifndef tbi
#define tbi(reg,bit)    reg ^= (_BV(bit))
#endif

#ifndef bit_is_set
#define bit_is_set(sfr, bit)   (_SFR_BYTE(sfr) & _BV(bit))
#endif


void timerInit() {
  //Wybranie trybu pracy CTC z TOP OCR1A
    cbi(TCCR1B,WGM13);
    sbi(TCCR1B,WGM12);
    cbi(TCCR1A,WGM11);
    cbi(TCCR1A,WGM10);
    

 
  //Wybranie dzielnika czestotliwosci - częstotliwość Atmega328: 16 MHz
  sbi(TCCR1B,CS12);
  cbi(TCCR1B,CS11);
  cbi(TCCR1B,CS10);

 
  //Zapisanie do OCR1A wartosci odpowiadajacej 0,005s
  OCR1A = 312;
 
  //Uruchomienie przerwania OCIE1A
  sbi(TIMSK1,OCIE1A);
  
  

}
volatile int liczba = 0;
int zeg[6]{0};
int bud[6]{0,0,0,0,0,0};
char text[20];
int budzi = 0;
bool wyjscie;
LCD_I2C Lcd = LCD_I2C(0x27);
int main() {
  sei();
  timerInit();
  cbi(EICRA, ISC00);
  sbi(EICRA, ISC01);
  sbi(EIMSK,INT0);
  //0x27 - adres urzadzenia na I2C
  sbi(DDRD,PD5);
  cbi(DDRD,PD2);
  sbi(PORTD,PD2);
  cbi(DDRD,PD3);
  sbi(PORTD,PD3);
  while(1)
  {
    if(bit_is_clear(PIND,PD3))
    {
      bud[5]++;
      if(bud[5] == 9)
      {
        bud[4] =0;
        bud[5] = 1;
      }
      
    }
    if(bit_is_clear(PIND,PD4))
    {
      break;
    }
  }
  
  _delay_ms(100);
  Lcd.backLightOn();


  uint8_t i = 0;

  while (1) {
    i = (i + 1) % 60;
    Lcd.clear();
    sprintf(text,"%d%d:%d%d:%d%d",zeg[0],zeg[1],zeg[2],zeg[3],zeg[4],zeg[5]);
    Lcd.goTo(0, 0);
    Lcd.writeText(text);
    sprintf(text,"%d%d:%d%d:%d%d",bud[0],bud[1],bud[2],bud[3],bud[4],bud[5]);
    Lcd.goTo(0, 1);
    Lcd.writeText(text);
    _delay_ms(1000);
  }
}
ISR(TIMER1_COMPA_vect) {
  cbi(PORTD,PD5);
  liczba++;
  if(liczba%312 == 1){
    zeg[5]++;
    if((zeg[5]+ 1)%10 == 1)
    {
      zeg[4]++;
      zeg[5] = 0;
      if((zeg[4] + 1)%6 == 1)
      {
        zeg[3]++;
        zeg[4] = 0;
        if((zeg[3]+ 1)%10 == 1)
        {
          zeg[2]++;
          zeg[3] = 0;
          if((zeg[2] + 1)%6 == 1)
      {
        zeg[1]++;
        zeg[0] = 0;
        if((zeg[0]+ 1)%10 == 1)
        {
          zeg[0]++;
          zeg[4] = 0;
          }
        }
          }
        }
        
  }
}
if(zeg[5] == bud[5] && zeg[4] == bud[4] && zeg[3] == zeg[3] && zeg[2] == zeg[2] &&zeg[1] == zeg[1] && zeg[0] == zeg[0])
{
    sbi(PORTD,PD5);
    wyjscie = true;
    while(1)
    {
      if(bit_is_clear(PIND, PD2))
      {
        break;
      }
    }
    cbi(PORTD,PD5);
    for(int i = 0; i<6; i++)
    {
      zeg[i] = 0;
    }
}
}
ISR(INT0_vect)
{
  wyjscie = false;
} 
  
