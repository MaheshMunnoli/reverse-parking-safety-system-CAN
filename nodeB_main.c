/*=============================================================
 * nodeB_main.c - Node B Entry Point
 * Project : Reverse Parking Safety System
 * Node    : Node B (Ultrasonic Sensor + CAN Response)
 * Board   : Rhydolabz LPC2129
 * Compiler: Keil uVision (ARM)
 *
 * Description:
 *   - Listens on CAN bus for RTR frame from Node A (ID: 0x6AB)
 *   - RTR frame received via CAN1 RX interrupt
 *   - On interrupt: measures distance using ultrasonic sensor
 *   - Sends distance value back to Node A as CAN data frame
 *   - Prints distance over UART0 for debug monitoring
 *
 * Pin Connections:
 *   P0.10 → ECHO pin of HC-SR04
 *   P0.11 → TRIG pin of HC-SR04
 *   P0.25 → CAN1 RD1 (RX)
 *   P0.0  → UART0 TXD
 *   P0.1  → UART0 RXD
 *=============================================================*/

#include "header.h"

/* Global variables - modified inside CAN RX ISR, so volatile */
volatile int  flag     = 0;    /* 1 = RTR frame received from Node A  */
volatile u32  GLO_ID   = 0;    /* Captured CAN message ID from ISR    */
volatile u32  GLO_DLC  = 0;    /* Captured DLC from ISR               */
volatile u32  curr_dist = 0;   /* Latest measured distance in cm      */

/* CAN message objects */
CAN1 tx;    /* Outgoing data frame to Node A */

int main(void)
{
    /* --- Peripheral Initialization --- */
    config_vic_for_rx_can1();       /* CAN1 RX interrupt setup           */
    can1_init();                    /* CAN1 @ 125 Kbps                   */

    uart_init(9600);                /* UART0 @ 9600 baud for debug       */
    uart_tx_str("Node B Ready: CAN1 & UART0 Initialized\r\n");

    IODIR0 |= TRIG_PIN;             /* P0.11 TRIG as output              */
    IODIR0 &= ~ECHO_PIN;            /* P0.10 ECHO as input               */
    IOCLR0  = TRIG_PIN;             /* TRIG LOW initially                */

    /* --- Main Loop --- */
    while (1)
    {
        if (flag == 1)
        {
            /* RTR received - measure distance from ultrasonic sensor */
            curr_dist = dis();

            if (curr_dist == SENSOR_FAULT)
            {
                uart_tx_str("WARNING: Ultrasonic sensor timeout!\r\n");
            }
            else
            {
                uart_tx_str("Distance: ");
                uart_integer(curr_dist);
                uart_tx_str(" cm\r\n");
            }

            /* Build and send data frame back to Node A */
            tx.id    = GLO_ID;      /* Reply to same ID Node A used     */
            tx.dlc   = GLO_DLC;     /* Same DLC as requested            */
            tx.rtr   = 0;           /* Data frame (not remote)          */
            tx.ff    = 0;           /* Standard frame                   */
            tx.byteA = curr_dist;   /* Distance in cm (or 999 on fault) */
            tx.byteB = 0;

            can1_tx(tx);            /* Transmit response to Node A      */

            flag = 0;               /* Clear flag - ready for next RTR  */
        }
    }

    return 0;
}
