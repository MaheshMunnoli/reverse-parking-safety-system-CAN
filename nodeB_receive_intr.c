/*=============================================================
 * nodeB_receive_intr.c - CAN1 RX Interrupt Handler
 * Project : Reverse Parking Safety System
 * Board   : Rhydolabz LPC2129
 *
 * Description:
 *   CAN1 RX interrupt fires when Node A sends an RTR frame.
 *   ISR extracts the message ID and DLC from the frame.
 *   Sets global flag so main loop measures distance and responds.
 *
 * Why interrupt-driven on Node B:
 *   Node B must respond ONLY when Node A requests (RTR frame).
 *   Using interrupt avoids busy-polling CAN bus continuously.
 *   This is more efficient and closer to real ECU design.
 *=============================================================*/

#include "header.h"

extern volatile int  flag;      /* Signals main loop: RTR received     */
extern volatile u32  GLO_ID;    /* Stores incoming CAN message ID      */
extern volatile u32  GLO_DLC;   /* Stores incoming DLC for reply       */

/*-------------------------------------------------------------
 * can1_rx_handler()
 * ISR: fires on CAN1 RX interrupt
 * Only processes RTR frames (bit 30 of RFS = 1)
 * Captures ID and DLC, then releases buffer and signals main
 *-------------------------------------------------------------*/
void can1_rx_handler(void) __irq
{
    u32 frame_info = C1RFS;        /* Read receive frame info register  */

    if (frame_info & (1 << 30))
    {
        /* RTR frame received from Node A */
        GLO_ID  = C1RID;                        /* Extract message ID  */
        GLO_DLC = (frame_info >> 16) & 0x0F;    /* Extract DLC         */
        flag    = 1;                            /* Signal main loop    */
    }

    C1CMR    = (1 << 2);           /* Release CAN RX buffer             */
    VICVectAddr = 0;               /* Signal end of interrupt to VIC    */
}

/*-------------------------------------------------------------
 * config_vic_for_rx_can1()
 * Configures VIC slot 0 for CAN1 RX interrupt (source 26)
 *-------------------------------------------------------------*/
void config_vic_for_rx_can1(void)
{
    VICIntSelect  = 0;                         /* All IRQ, no FIQ       */
    VICVectCntl0  = 26 | (1 << 5);            /* Slot 0: CAN1 RX (26)  */
    VICVectAddr0  = (u32)can1_rx_handler;      /* ISR address           */
    VICIntEnable  = (1 << 26);                 /* Enable CAN1 interrupt  */
}
