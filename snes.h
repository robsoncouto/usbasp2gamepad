
#ifndef _SNES_H
#define _SNES_H

#define JOYPORT PORTB
#define JOYPIN PINB
#define JOYDDR DDRB


//#define MISO 4
//#define SCK  5
//#define MOSI 3
//#define RESET 2

#define CLOCK 5
#define LATCH 4
#define DATA 3

void ioInit(void);
uint16_t readButtons(void);


/*
Data available at https://www.gamefaqs.com/snes/916396-super-nintendo/faqs/5395

Clock Cycle     Button Reported
===========     ===============
1               B
2               Y
3               Select
4               Start
5               Up on joypad
6               Down on joypad
7               Left on joypad
8               Right on joypad
9               A
10              X
11              L
12              R
13              none (always high)
14              none (always high)
15              none (always high)
16              none (always high)
*/

#endif
