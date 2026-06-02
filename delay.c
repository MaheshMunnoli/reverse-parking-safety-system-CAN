/*=============================================================
 * delay.c - Timer0 Based Blocking Delay Functions
 * Project : Reverse Parking Safety System
 * Board   : Rhydolabz LPC2129
 * Compiler: Keil uVision (ARM)
 *
 * Description:
 *   Uses Timer0 for blocking delays.
 *   PCLK assumed = 60MHz (VPBDIV=1)
 *   Timer0 is dedicated to delays only - not used for interrupts.
 *=============================================================*/

#include "header.h"

/*-------------------------------------------------------------
 * delay_ms()
 * Blocking delay in milliseconds
 * Prescaler: 60000-1 → 1 tick = 1ms at 60MHz PCLK
 *-------------------------------------------------------------*/
void delay_ms(u32 ms)
{
    T0PC  = 0;
    T0TC  = 0;
    T0PR  = 60000 - 1;    /* 1 tick = 1ms                            */
    T0TCR = 1;            /* Start Timer0                            */
    while (T0TC < ms);    /* Wait until tick count reaches ms value  */
    T0TCR = 0;            /* Stop Timer0                             */
}

/*-------------------------------------------------------------
 * delay_us()
 * Blocking delay in microseconds
 * Prescaler: 60-1 → 1 tick = 1us at 60MHz PCLK
 *-------------------------------------------------------------*/
void delay_us(u32 us)
{
    T0PR  = 60 - 1;       /* 1 tick = 1us                            */
    T0TC  = 0;
    T0PC  = 0;
    T0TCR = 1;            /* Start Timer0                            */
    while (T0TC < us);    /* Wait until tick count reaches us value  */
    T0TCR = 0;            /* Stop Timer0                             */
}

/*-------------------------------------------------------------
 * delay_sec()
 * Blocking delay in seconds
 * Prescaler: 60000000-1 → 1 tick = 1s at 60MHz PCLK
 *-------------------------------------------------------------*/
void delay_sec(u32 sec)
{
    T0PC  = 0;
    T0TC  = 0;
    T0PR  = 60000000 - 1; /* 1 tick = 1s                             */
    T0TCR = 1;            /* Start Timer0                            */
    while (T0TC < sec);   /* Wait until tick count reaches sec value */
    T0TCR = 0;            /* Stop Timer0                             */
}
