#include<avr/io.h>
#include <util/delay.h>
#include "snes.h"
void ioInit(void){
  JOYDDR|=(1<<LATCH)|(1<<CLOCK);
  JOYDDR&=~(1<<DATA);
}
uint16_t readButtons(void){
  uint16_t buttonData=0;
  JOYPORT|=(1<<CLOCK)|(1<<LATCH);
  _delay_us(12);
  JOYPORT&=~(1<<LATCH);
  for(int i=0;i<16;i++){
    _delay_us(6);
    if((JOYPIN&(1<<DATA)))
      buttonData|=1<<i;
    JOYPORT&=~(1<<CLOCK);
    _delay_us(6);
    JOYPORT|=(1<<CLOCK);
  }
  return ~buttonData;
}
