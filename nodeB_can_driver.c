/*=============================================================
 * nodeB_can_driver.c - CAN1 Driver for Node B
 * Project : Reverse Parking Safety System
 * Board   : Rhydolabz LPC2129
 *
 * Description:
 *   Initializes CAN1 at 125 Kbps with RX interrupt enabled.
 *   Node B uses interrupt-driven RX (unlike Node A polling RX).
 *   Provides blocking TX for sending distance data back to Node A.
 *=============================================================*/

#include "header.h"

/* CAN Global Status Register bit macros */
#define TCS     ((C1GSR >> 3) & 1)   /* Transmit Complete Status */
#define RBS     (C1GSR & 1)          /* Receive Buffer Status    */

/*-------------------------------------------------------------
 * can1_init()
 * Initializes CAN1 at 125 Kbps with RX interrupt enabled.
 * Node B enables C1IER to trigger ISR on RTR frame arrival.
 *-------------------------------------------------------------*/
void can1_init(void)
{
    VPBDIV = 1;                    /* PCLK = CCLK = 60 MHz              */

    PINSEL1 |= 0x00040000;         /* P0.25 → RD1 (CAN1 RX)            */

    C1MOD = 1;                     /* Enter reset mode                  */

    C1BTR = 0x001C001D;            /* 125 Kbps @ 60 MHz PCLK
                                      BRP=29, TSEG1=14, TSEG2=2        */

    AFMR = 2;                      /* Bypass acceptance filter -
                                      accept all incoming messages      */

    C1IER = 0x1;                   /* Enable CAN1 RX interrupt          */

    C1MOD = 0;                     /* Release reset mode - CAN active   */
}

/*-------------------------------------------------------------
 * can1_rx()
 * Polling-based RX - used if interrupt-driven approach is bypassed.
 * Waits until a message is available then reads it.
 *-------------------------------------------------------------*/
void can1_rx(CAN1 *ptr)
{
    while (RBS == 0);              /* Wait for message in RX buffer     */

    ptr->id  = C1RID;
    ptr->rtr = (C1RFS >> 30) & 1;
    ptr->dlc = (C1RFS >> 16) & 0xF;
    ptr->ff  = (C1RFS >> 31) & 1;

    if (ptr->rtr == 0)
    {
        ptr->byteA = C1RDA;
        ptr->byteB = C1RDB;
    }

    C1CMR = 0x4;                   /* Release RX buffer                 */
}

/*-------------------------------------------------------------
 * can1_tx()
 * Transmits a CAN data frame.
 * Used by Node B to send distance value back to Node A.
 * Blocking - waits until transmission is complete.
 *-------------------------------------------------------------*/
void can1_tx(CAN1 v)
{
    C1TID1 = v.id;                 /* Set message ID                    */

    C1TFI1 = (v.dlc << 16);       /* Set DLC, RTR=0, FF=0              */

    if (v.rtr == 0)
    {
        C1TDA1 = v.byteA;          /* Distance value in bytes 0-3       */
        C1TDB1 = v.byteB;          /* Reserved / zero                   */
    }
    else
    {
        C1TFI1 |= (1 << 30);       /* Set RTR bit if needed             */
    }

    C1CMR = 0x21;                  /* Select TX Buffer 1 & transmit     */

    while (TCS == 0);              /* Wait until transmission complete  */
}
