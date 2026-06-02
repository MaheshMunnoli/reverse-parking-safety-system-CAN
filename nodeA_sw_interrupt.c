/*=============================================================
 * nodeA_sw_interrupt.c - External Interrupt 0 (Gear Detection)
 * Project : Reverse Parking Safety System
 * Board   : Rhydolabz LPC2129
 *
 * Description:
 *   EINT0 on P0.16 detects reverse gear switch press.
 *   Falling edge triggered.
 *   Toggles global flag on each press.
 *   flag=1 → reverse gear engaged
 *   flag=0 → reverse gear disengaged
 *=============================================================*/

#include "header.h"

extern volatile int flag;   /* Shared with main and timer ISR */

/*-------------------------------------------------------------
 * eint0_handler()
 * ISR for External Interrupt 0 (P0.16)
 * Toggles reverse gear flag on falling edge
 *-------------------------------------------------------------*/
void eint0_handler(void) __irq
{
    VICIntEnClr = (1 << 14);   /* Disable EINT0 interrupt temporarily  */

    flag ^= 1;                 /* Toggle reverse gear flag              */

    delay_ms(200);             /* Debounce delay                        */

    EXTINT = 1;                /* Clear EINT0 pending flag              */

    VICVectAddr = 0;           /* Signal end of interrupt to VIC        */

    VICIntEnable = (1 << 14);  /* Re-enable EINT0 interrupt             */

    if (flag == 1)
        uart_tx_str("Reverse Gear Engaged\r\n");
    else
        uart_tx_str("Reverse Gear Disengaged\r\n");
}

/*-------------------------------------------------------------
 * config_vic_for_eint0()
 * Configures VIC and EINT0 for falling edge interrupt on P0.16
 *-------------------------------------------------------------*/
void config_vic_for_eint0(void)
{
    PINSEL1     |= 1;          /* P0.16 → EINT0 function               */
    EXTMODE      = 1;          /* Edge-sensitive triggering             */
    EXTPOLAR     = 0;          /* Falling edge trigger                  */
    VICIntSelect = 0;          /* All interrupts as IRQ (not FIQ)       */
    VICVectCntl0 = 14 | (1 << 5); /* Slot 0: source 14 (EINT0), enable */
    VICVectAddr0 = (u32)eint0_handler; /* ISR address                  */
    VICIntEnable = (1 << 14);  /* Enable EINT0 in VIC                   */
}
