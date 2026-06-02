/*=============================================================
 * header.h - Node A
 * Project : Reverse Parking Safety System
 * Node    : Node A (Gear Detection + Buzzer Control)
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
#define LED_PIN         (1<<18)   /* P0.18 - LED indicator     */
#define BUZZER_PIN      (1<<21)   /* P0.21 - Buzzer output     */

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
 * CAN Driver Declarations (nodeA_can_driver.c)
 *-------------------------------------------------------------*/
extern void can1_init(void);
extern void can1_tx(CAN1 v);
extern void can1_rx(CAN1 *ptr);

/*-------------------------------------------------------------
 * UART Driver Declarations (nodeA_uart_driver.c)
 *-------------------------------------------------------------*/
extern void uart_init(u32 baud);
extern void uart_tx(u8 data);
extern u8   uart_rx(void);
extern void uart_tx_str(char *str);
extern void uart_integer(int num);
extern void uart_float(float num);

/*-------------------------------------------------------------
 * Interrupt & Timer Declarations
 * (nodeA_sw_interrupt.c, nodeA_timer_intr.c)
 *-------------------------------------------------------------*/
extern void config_vic_for_eint0(void);
extern void timer1_init(void);
extern void update_buzzer(int distance);

#endif /* HEADER_H */
