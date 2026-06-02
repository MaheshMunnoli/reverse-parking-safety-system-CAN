/*=============================================================
 * nodeB_ultrasonic.c - HC-SR04 Ultrasonic Distance Measurement
 * Project : Reverse Parking Safety System
 * Board   : Rhydolabz LPC2129
 *
 * Description:
 *   Measures distance using HC-SR04 ultrasonic sensor.
 *   Uses Timer1 to measure ECHO pulse width in microseconds.
 *   Distance = (pulse_width_us * 0.0343) / 2  (cm)
 *
 * Pin Connections:
 *   P0.11 → TRIG (output) - 10us trigger pulse
 *   P0.10 → ECHO (input)  - pulse width = time of flight
 *
 * Fault Handling:
 *   Returns SENSOR_FAULT (999) if:
 *   - ECHO does not go HIGH within timeout (sensor not connected)
 *   - ECHO does not go LOW within timeout (object too close/far)
 *=============================================================*/

#include "header.h"

/*-------------------------------------------------------------
 * dis()
 * Measures and returns distance in cm.
 * Returns SENSOR_FAULT (999) on timeout.
 *
 * Timing:
 *   1. Send 10us HIGH pulse on TRIG
 *   2. Wait for ECHO to go HIGH (start of pulse)
 *   3. Start Timer1 counting in microseconds
 *   4. Wait for ECHO to go LOW (end of pulse)
 *   5. Read Timer1 count = pulse width in us
 *   6. Distance = (count * 0.0343) / 2
 *-------------------------------------------------------------*/
unsigned int dis(void)
{
    unsigned int t;
    unsigned int timeout;

    /* Configure Timer1 for microsecond counting (prescaler = 60-1) */
    T1PR  = 60 - 1;               /* 1 tick = 1us @ 60MHz PCLK         */
    T1TC  = 0;
    T1PC  = 0;
    T1TCR = 0x02;                 /* Reset Timer1 (hold in reset)       */

    /* Step 1: Send TRIG pulse - LOW, then 10us HIGH, then LOW */
    IOCLR0 = TRIG_PIN;
    delay_us(2);
    IOSET0 = TRIG_PIN;
    delay_us(10);
    IOCLR0 = TRIG_PIN;

    /* Step 2: Wait for ECHO to go HIGH with timeout */
    timeout = 100000;
    while (!(IOPIN0 & ECHO_PIN) && timeout--)
    {
        /* Waiting for ECHO HIGH */
    }
    if (timeout == 0)
    {
        return SENSOR_FAULT;      /* Sensor not responding              */
    }

    /* Step 3: Start Timer1 - begin counting pulse width */
    T1TCR = 0x01;

    /* Step 4: Wait for ECHO to go LOW with timeout */
    timeout = 100000;
    while ((IOPIN0 & ECHO_PIN) && timeout--)
    {
        /* Counting pulse width */
    }

    /* Step 5: Stop Timer1 */
    T1TCR = 0;
    t = T1TC;                     /* Pulse width in microseconds        */

    /* Step 6: Calculate distance in cm
       Speed of sound = 343 m/s = 0.0343 cm/us
       Distance = (time * speed) / 2  (divide by 2 for round trip) */
    return (unsigned int)((t * 0.0343) / 2);
}
