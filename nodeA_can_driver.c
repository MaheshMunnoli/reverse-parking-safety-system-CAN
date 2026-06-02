/*=============================================================
 * nodeA_can_driver.c - CAN1 Driver for Node A
 * Project : Reverse Parking Safety System
 * Board   : Rhydolabz LPC2129
 *
 * Description:
 *   Initializes CAN1 peripheral at 125 Kbps.
 *   Provides blocking TX and RX functions.
 *   Node A uses CAN1 (P0.25 = RD1).
 *=============================================================*/

#include "header.h"

/* CAN Global Status Register bit macros */
#define TCS     ((C1GSR >> 3) & 1)   /* Transmit Complete Status */
#define RBS     (C1GSR & 1)          /* Receive Buffer Status    */

/*-------------------------------------------------------------
 * can1_init()
 * Initializes CAN1 at 125 Kbps, accepts all messages
 *-------------------------------------------------------------*/
void can1_init(void)
{
    VPBDIV = 1;                    /* PCLK = CCLK = 60 MHz          */

    PINSEL1 |= 0x00040000;         /* P0.25 -> RD1 (CAN1 RX)        */

    C1MOD = 1;                     /* Enter reset mode               */

    C1BTR = 0x001C001D;            /* 125 Kbps @ 60 MHz PCLK
                                      BRP=29, TSEG1=14, TSEG2=2     */

    AFMR = 2;                      /* Bypass acceptance filter -
                                      accept all incoming messages   */

    C1MOD = 0;                     /* Release reset mode - CAN active*/
}

/*-------------------------------------------------------------
 * can1_tx()
 * Transmits a CAN message (data frame or RTR frame)
 * Uses TX Buffer 1, blocking until transmission complete
 *-------------------------------------------------------------*/
void can1_tx(CAN1 v)
{
    C1TID1 = v.id;                 /* Set message ID                 */

    C1TFI1 = (v.dlc << 16);       /* Set DLC, RTR=0, FF=0 initially */

    if (v.rtr == 0)
    {
        /* Data frame - load payload */
        C1TDA1 = v.byteA;          /* Bytes 0-3                      */
        C1TDB1 = v.byteB;          /* Bytes 4-7                      */
    }
    else
    {
        /* Remote frame - set RTR bit */
        C1TFI1 |= (1 << 30);
    }

    C1CMR = 0x21;                  /* Select TX Buffer 1 & transmit  */

    while (TCS == 0);              /* Wait until transmission done   */
}

/*-------------------------------------------------------------
 * can1_rx()
 * Receives a CAN message into the provided struct pointer.
 * Blocking - waits until a message arrives in RX buffer.
 *-------------------------------------------------------------*/
void can1_rx(CAN1 *ptr)
{
    while (RBS == 0);              /* Wait for message in RX buffer  */

    ptr->id  = C1RID;              /* Extract message ID             */
    ptr->rtr = (C1RFS >> 30) & 1; /* Extract RTR bit                */
    ptr->dlc = (C1RFS >> 16) & 0xF; /* Extract DLC                  */
    ptr->ff  = (C1RFS >> 31) & 1; /* Extract frame format bit       */

    if (ptr->rtr == 0)
    {
        /* Data frame - read payload */
        ptr->byteA = C1RDA;        /* Bytes 0-3                      */
        ptr->byteB = C1RDB;        /* Bytes 4-7                      */
    }

    C1CMR = 0x4;                   /* Release RX buffer              */
}
