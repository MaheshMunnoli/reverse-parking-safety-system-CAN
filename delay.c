/*=============================================================
 * delay.c - Timer0 Based Blocking Delay Functions
 * Project : Reverse Parking Safety System
 * Board   : Rhydolabz LPC2129
 * Compiler: Keil uVision (ARM)
 *
 * Note: Timer0 is used exclusively for delays.
 *       Timer1 is used by ultrasonic sensor pulse measurement.
 *       Do NOT use Timer0 for anything else in Node B.
 *=============================================================*/

#include "header.h"

/*-------------------------------------------------------------
 * delay_ms()
 * Blocking delay in milliseconds
 *-------------------------------------------------------------*/
void delay_ms(u32 ms)
{
    T0PC  = 0;
    T0TC  = 0;
    T0PR  = 60000 - 1;    /* 1 tick = 1ms @ 60MHz PCLK               */
    T0TCR = 1;
    while (T0TC < ms);
    T0TCR = 0;
}

/*-------------------------------------------------------------
 * delay_us()
 * Blocking delay in microseconds
 *-------------------------------------------------------------*/
void delay_us(u32 us)
{
    T0PR  = 60 - 1;       /* 1 tick = 1us @ 60MHz PCLK               */
    T0TC  = 0;
    T0PC  = 0;
    T0TCR = 1;
    while (T0TC < us);
    T0TCR = 0;
}

/*-------------------------------------------------------------
 * delay_sec()
 * Blocking delay in seconds
 *-------------------------------------------------------------*/
void delay_sec(u32 sec)
{
    T0PC  = 0;
    T0TC  = 0;
    T0PR  = 60000000 - 1; /* 1 tick = 1s @ 60MHz PCLK                */
    T0TCR = 1;
    while (T0TC < sec);
    T0TCR = 0;
}
