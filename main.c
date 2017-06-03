/* Name: main.c
 * Project: hid-mouse, a very simple HID example
 * Author: Christian Starkjohann
 * Creation Date: 2008-04-07
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 */

/*
This example should run on most AVRs with only little changes. No special
hardware resources except INT0 are used. You may have to change usbconfig.h for
different I/O pins for USB. Please note that USB D+ must be the INT0 pin, or
at least be connected to INT0 as well.

We use VID/PID 0x046D/0xC00E which is taken from a Logitech mouse. Don't
publish any hardware using these IDs! This is for demonstration only!
*/

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "snes.h"

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

PROGMEM const char usbHidReportDescriptor[42] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x08,                    //     USAGE_MAXIMUM (Button 8)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x08,                    //     REPORT_COUNT (8)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                           // END_COLLECTION
    0xc0                           // END_COLLECTION
};


typedef struct{
    uchar   buttonMask;
    char    X;
    char    Y;
}report_t;

static report_t reportBuffer;

/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
}

void packData(report_t * report_buff){
  uint16_t buttons=0x00;
  report_buff->buttonMask=0;
  report_buff->X=0;
  report_buff->Y=0;
  buttons=readButtons();
  if(buttons&(1<<0)){//B
    report_buff->buttonMask|=(1<<2);//Button 3
  }
  if(buttons&(1<<1)){//Y
    report_buff->buttonMask|=(1<<3);//Button 4
  }
  if(buttons&(1<<2)){//Select
    report_buff->buttonMask|=(1<<6);//Button 7
  }
  if(buttons&(1<<3)){//Start
    report_buff->buttonMask|=(1<<7);//Button 8
  }
  if(buttons&(1<<4)){//Up
    report_buff->Y=-126;//Button 4
  }else if(buttons&(1<<5)){//Down
    report_buff->Y|=126;//Button 4
  }
  if(buttons&(1<<6)){//Left
    report_buff->X=-126;//Button 4
  }else if(buttons&(1<<7)){//Right
    report_buff->X=126;//Button 4
  }
  if(buttons&(1<<8)){//A
    report_buff->buttonMask|=(1<<1);//Button 2
  }
  if(buttons&(1<<9)){//X
    report_buff->buttonMask|=(1<<0);//Button 1
  }
  if(buttons&(1<<10)){//L
    report_buff->buttonMask|=(1<<4);//Button 5
  }
  if(buttons&(1<<11)){//R
    report_buff->buttonMask|=(1<<5);//Button 6
  }
}
/* ------------------------------------------------------------------------- */

int __attribute__((noreturn)) main(void)
{
uchar   i;

    wdt_enable(WDTO_1S);
    /* Even if you don't use the watchdog, turn it off here. On newer devices,
     * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
     */
    /* RESET status: all port bits are inputs without pull-up.
     * That's the way we need D+ and D-. Therefore we don't need any
     * additional hardware initialization.
     */
    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();
    sei();
    ioInit();
    for(;;){                /* main event loop */
        wdt_reset();
        usbPoll();
        if(usbInterruptIsReady()){
            //calls the function to read the data from the joypad and make the report
            packData(&reportBuffer);
            /* called after every poll of the interrupt endpoint */
            usbSetInterrupt((void *)&reportBuffer, sizeof(reportBuffer));
        }

    }
}
