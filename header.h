/*=============================================================
 * header.h - Node B
 * Project : Reverse Parking Safety System
 * Node    : Node B (Ultrasonic Sensor + CAN Response)
 * Board   : Rhydolabz LPC2129
 * Compiler: Keil uVision (ARM)
 * Date    : 2025
 *=============================================================*/

#ifndef HEADER_H
#define HEADER_H

#include <lpc21xx.h>

/*-------------------------------------------------------------
 * Typedefs
 *-------------------------------------------------------------*/
typedef unsigned int        u32;
typedef signed int          s32;
typedef unsigned short int  u16;
typedef signed short int    s16;
typedef unsigned char       u8;
typedef signed char         s8;

/*-------------------------------------------------------------
 * Pin Definitions
 *-------------------------------------------------------------*/
#define TRIG_PIN    (1 << 11)   /* P0.11 - Ultrasonic TRIG output    */
#define ECHO_PIN    (1 << 10)   /* P0.10 - Ultrasonic ECHO input     */

/*-------------------------------------------------------------
 * Sensor Fault Code
 * Returned by dis() when ECHO signal not detected (timeout)
 *-------------------------------------------------------------*/
#define SENSOR_FAULT    999

/*-------------------------------------------------------------
 * CAN Message Structure
 *-------------------------------------------------------------*/
typedef struct
{
    u32 id;       /* CAN message ID                 */
    u32 byteA;    /* Data bytes 0-3 (lower 4 bytes) */
    u32 byteB;    /* Data bytes 4-7 (upper 4 bytes) */
    u8  rtr;      /* 0=Data frame, 1=Remote frame   */
    u8  dlc;      /* Data length code (0-8)         */
    u8  ff;       /* 0=Standard, 1=Extended frame   */
} CAN1;

/*-------------------------------------------------------------
 * Delay Function Declarations (delay.c)
 *-------------------------------------------------------------*/
extern void delay_ms(u32 ms);
extern void delay_us(u32 us);
extern void delay_sec(u32 sec);

/*-------------------------------------------------------------
 * CAN Driver Declarations (nodeB_can_driver.c)
 *-------------------------------------------------------------*/
extern void can1_init(void);
extern void can1_tx(CAN1 v);
extern void can1_rx(CAN1 *ptr);

/*-------------------------------------------------------------
 * CAN RX Interrupt Declarations (nodeB_receive_intr.c)
 *-------------------------------------------------------------*/
extern void config_vic_for_rx_can1(void);

/*-------------------------------------------------------------
 * UART Driver Declarations (nodeB_uart_driver.c)
 *-------------------------------------------------------------*/
extern void uart_init(u32 baud);
extern void uart_tx(u8 data);
extern u8   uart_rx(void);
extern void uart_tx_str(char *str);
extern void uart_integer(int num);
extern void uart_float(double num);

/*-------------------------------------------------------------
 * Ultrasonic Sensor Declaration (nodeB_ultrasonic.c)
 *-------------------------------------------------------------*/
extern unsigned int dis(void);   /* Returns distance in cm,
                                    999 on sensor fault/timeout  */

#endif /* HEADER_H */
