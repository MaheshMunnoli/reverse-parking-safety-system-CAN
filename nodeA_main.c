/*=============================================================
 * nodeA_main.c - Node A Entry Point
 * Project : Reverse Parking Safety System
 * Node    : Node A (Gear Detection + Buzzer Control)
 * Board   : Rhydolabz LPC2129
 * Compiler: Keil uVision (ARM)
 *
 * Description:
 *   - Detects reverse gear via EINT0 (external interrupt)
 *   - Sends RTR frame to Node B over CAN bus (ID: 0x6AB)
 *   - Receives distance data from Node B
 *   - Drives adaptive buzzer via Timer1 interrupt
 *   - Prints distance over UART0 (9600 baud)
 *=============================================================*/

#include "header.h"

/* Global flags - modified inside ISRs, so volatile */
volatile int flag = 0;   /* 1 = reverse gear engaged, 0 = not engaged */
volatile int val  = 0;   /* distance in cm received from Node B       */

/* CAN message objects */
CAN1 v2;   /* outgoing RTR frame to Node B  */
CAN1 r1;   /* incoming data frame from Node B */

int main(void)
{
    /* --- Peripheral Initialization --- */
    config_vic_for_eint0();         /* EINT0: reverse gear switch    */

    IODIR0 |= LED_PIN;              /* LED as output                 */
    IOSET0  = LED_PIN;              /* LED OFF (active low)          */

    IODIR0 |= BUZZER_PIN;           /* Buzzer as output              */
    IOSET0  = BUZZER_PIN;           /* Buzzer OFF (active low)       */

    can1_init();                    /* CAN1 @ 125 Kbps               */
    uart_init(9600);                /* UART0 @ 9600 baud             */
    uart_tx_str("Node A Ready: CAN1 & UART0 Initialized\r\n");

    timer1_init();                  /* Timer1: buzzer zone control   */

    /* --- Main Loop --- */
    while (1)
    {
        if (flag == 1)
        {
            /* Reverse gear detected - indicate with LED ON */
            IOCLR0 = LED_PIN;

            /* Build RTR frame - request distance from Node B */
            v2.id  = 0x6AB;
            v2.rtr = 1;        /* Remote frame                      */
            v2.dlc = 8;

            can1_tx(v2);       /* Send RTR frame to Node B          */

            can1_rx(&r1);      /* Wait and receive distance data    */

            val = r1.byteA;    /* Distance value in cm              */

            /* Check for ultrasonic sensor fault from Node B */
            if (val == 999)
            {
                uart_tx_str("ALERT: Ultrasonic sensor fault on Node B!\r\n");
                /* Keep buzzer continuous on sensor fault */
                IOCLR0 = BUZZER_PIN;
                continue;
            }

            /* Print distance and update buzzer zone */
            uart_tx_str("Distance: ");
            uart_integer(val);
            uart_tx_str(" cm\r\n");

            update_buzzer(val);
        }
        else
        {
            /* Not in reverse - LED and buzzer OFF */
            IOSET0 = LED_PIN;
            IOSET0 = BUZZER_PIN;
        }
    }

    return 0;
}
