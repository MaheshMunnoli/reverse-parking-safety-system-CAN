/*=============================================================
 * nodeA_timer_intr.c - Timer1 Interrupt (Buzzer Zone Control)
 * Project : Reverse Parking Safety System
 * Board   : Rhydolabz LPC2129
 *
 * Description:
 *   Timer1 fires every 1ms.
 *   Controls buzzer beep rate based on distance zone:
 *
 *   Zone 1: > 400 cm  → Silent (no beep)
 *   Zone 2: 301-400cm → Slow beep   (threshold=800ms)
 *   Zone 3: 201-300cm → Medium beep (threshold=400ms)
 *   Zone 4:  51-200cm → Fast beep   (threshold=150ms)
 *   Zone 5:   0-50cm  → Continuous  (always ON)
 *=============================================================*/

#include "header.h"

extern volatile int flag;           /* Reverse gear flag from EINT0 ISR */

volatile u32 counter      = 0;      /* ms counter for buzzer toggle     */
volatile u32 threshold    = 1000;   /* Toggle interval in ms            */
volatile u32 buzzer_state = 0;      /* Current buzzer on/off state      */
volatile u32 continuous   = 0;      /* 1 = continuous buzzer (zone 5)   */

/*-------------------------------------------------------------
 * timer1_handler()
 * ISR: fires every 1ms
 * Manages buzzer toggling based on zone threshold
 *-------------------------------------------------------------*/
void timer1_handler(void) __irq
{
    T1IR = 1;                       /* Clear Timer1 match interrupt     */

    counter++;

    if (flag == 0)
    {
        /* Not in reverse - silence buzzer and reset counter */
        IOSET0  = BUZZER_PIN;
        counter = 0;
    }
    else if (continuous == 1)
    {
        /* Zone 5: < 50cm - continuous buzzer ON */
        IOCLR0 = BUZZER_PIN;
    }
    else
    {
        /* Zones 2-4: toggle buzzer at threshold interval */
        if (counter >= threshold)
        {
            buzzer_state = !buzzer_state;

            if (buzzer_state)
                IOCLR0 = BUZZER_PIN;   /* Buzzer ON  */
            else
                IOSET0 = BUZZER_PIN;   /* Buzzer OFF */

            counter = 0;
        }
    }

    VICVectAddr = 0;                /* Signal end of interrupt to VIC   */
}

/*-------------------------------------------------------------
 * timer1_init()
 * Initializes Timer1 for 1ms tick interrupt
 *-------------------------------------------------------------*/
void timer1_init(void)
{
    /* PCLK lookup: index 0=15MHz, 1=60MHz, 2=30MHz */
    s32 pclk_table[] = {15, 60, 30, 15, 15};
    u32 pclk_khz     = pclk_table[VPBDIV % 4] * 1000;

    T1PC  = 0;
    T1PR  = pclk_khz - 1;          /* Prescaler: 1 tick = 1ms          */
    T1MR0 = 1;                     /* Match every 1 tick (1ms)         */
    T1MCR = 3;                     /* Interrupt and reset on match      */

    VICVectAddr5  = (u32)timer1_handler;  /* Slot 5: Timer1 ISR        */
    VICVectCntl5  = 5 | (1 << 5);        /* Source 5 (Timer1), enable  */
    VICIntEnable |= (1 << 5);            /* Enable Timer1 interrupt     */

    T1TCR = 1;                     /* Start Timer1                      */
}

/*-------------------------------------------------------------
 * update_buzzer()
 * Called from main after each distance reading.
 * Updates threshold and continuous flag based on distance zone.
 *-------------------------------------------------------------*/
void update_buzzer(int distance)
{
    if (flag == 1)
    {
        if (distance > 400)
        {
            /* Zone 1: Silent */
            threshold  = 1000;
            continuous = 0;
            IOSET0     = BUZZER_PIN;   /* Ensure buzzer OFF            */
        }
        else if (distance > 300)
        {
            /* Zone 2: Slow beep */
            threshold  = 800;
            continuous = 0;
        }
        else if (distance > 200)
        {
            /* Zone 3: Medium beep */
            threshold  = 400;
            continuous = 0;
        }
        else if (distance > 50)
        {
            /* Zone 4: Fast beep */
            threshold  = 150;
            continuous = 0;
        }
        else
        {
            /* Zone 5: Continuous - danger zone */
            continuous = 1;
        }
    }
}
