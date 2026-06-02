/*=============================================================
 * nodeB_uart_driver.c - UART0 Driver for Node B
 * Project : Reverse Parking Safety System
 * Board   : Rhydolabz LPC2129
 *
 * Description:
 *   UART0 at configurable baud rate, 8N1 format.
 *   Used for debug output (distance values, status messages).
 *=============================================================*/

#include "header.h"

/* UART0 Line Status Register bit macros */
#define THRE    ((U0LSR >> 5) & 1)   /* TX Holding Register Empty */
#define RDR     (U0LSR & 1)          /* Receiver Data Ready       */

/*-------------------------------------------------------------
 * uart_init()
 * Initializes UART0 at given baud rate, 8N1 format
 *-------------------------------------------------------------*/
void uart_init(u32 baud)
{
    int pclk, divisor;
    int pclk_table[] = {15, 60, 30};   /* PCLK lookup for VPBDIV 0,1,2 */

    pclk    = pclk_table[VPBDIV] * 1000000;
    divisor = pclk / (16 * baud);

    PINSEL0 |= 0x05;               /* P0.0=TXD0, P0.1=RXD0             */
    U0LCR    = 0x83;               /* DLAB=1, 8-bit, no parity, 1 stop  */
    U0DLL    = divisor & 0xFF;     /* Baud rate LSB                     */
    U0DLM    = (divisor >> 8) & 0xFF; /* Baud rate MSB                  */
    U0LCR    = 0x03;               /* DLAB=0, 8N1 active                */
}

/*-------------------------------------------------------------
 * uart_tx()
 * Transmits single byte, blocking
 *-------------------------------------------------------------*/
void uart_tx(u8 data)
{
    U0THR = data;
    while (THRE == 0);
}

/*-------------------------------------------------------------
 * uart_rx()
 * Receives single byte, blocking
 *-------------------------------------------------------------*/
u8 uart_rx(void)
{
    while (RDR == 0);
    return U0RBR;
}

/*-------------------------------------------------------------
 * uart_tx_str()
 * Transmits null-terminated string
 *-------------------------------------------------------------*/
void uart_tx_str(char *str)
{
    while (*str != '\0')
    {
        uart_tx(*str++);
    }
}

/*-------------------------------------------------------------
 * uart_integer()
 * Prints signed integer over UART
 *-------------------------------------------------------------*/
void uart_integer(int num)
{
    int arr[10], i = 0;

    if (num == 0)
    {
        uart_tx('0');
        return;
    }
    if (num < 0)
    {
        uart_tx('-');
        num = -num;
    }
    while (num)
    {
        arr[i++] = (num % 10) + '0';
        num /= 10;
    }
    for (i = i - 1; i >= 0; i--)
    {
        uart_tx(arr[i]);
    }
}

/*-------------------------------------------------------------
 * uart_float()
 * Prints double with 6 decimal places over UART
 *-------------------------------------------------------------*/
void uart_float(double num)
{
    int arr[10], i = 0;
    int mul = 1000000;
    long long int temp = 0;

    if (num == 0)
    {
        uart_tx_str("0.000000");
        return;
    }
    if (num < 0)
    {
        uart_tx('-');
        num = -num;
    }
    if (num > 0 && num < 1)
    {
        uart_tx_str("0.");
    }
    if (num > 0)
    {
        temp = (long long int)(num * mul);
        while (temp)
        {
            arr[i++] = (temp % 10) + '0';
            temp /= 10;
        }
        for (i = i - 1; i >= 0; i--)
        {
            uart_tx(arr[i]);
            if (i == 6) uart_tx('.');
        }
    }
}
