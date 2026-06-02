# Reverse Parking Safety System using CAN Bus & Ultrasonic Sensor

An embedded systems project implementing a dual-node CAN Bus communication
system for real-time reverse parking assistance on the LPC2129 ARM7 microcontroller.

---

## System Overview

```
  [ Reverse Gear Switch ]
           |
        EINT0
           |
       [ NODE A ]  ----CAN Bus (125 Kbps)---->  [ NODE B ]
    LPC2129 (ARM7)   <---CAN Bus (125 Kbps)----  LPC2129 (ARM7)
           |                                          |
       [ Buzzer ]                           [ HC-SR04 Ultrasonic ]
```

| Node | Role |
|------|------|
| Node A | Detects reverse gear via interrupt, sends RTR frame, receives distance, controls buzzer |
| Node B | Listens for RTR frame via interrupt, measures distance, sends data frame back |

---

## Features

- Dual-node CAN Bus communication at 125 Kbps
- Interrupt-driven reverse gear detection (EINT0) on Node A
- Interrupt-driven CAN RX on Node B (RTR frame detection)
- Real-time distance measurement using HC-SR04 ultrasonic sensor
- Adaptive 5-zone buzzer alert system via Timer1 interrupt
- Sensor fault detection — returns code 999 on ultrasonic timeout
- UART0 debug output on both nodes at 9600 baud

---

## Buzzer Zone Logic (Node A)

| Zone | Distance    | Buzzer Behavior     |
|------|-------------|---------------------|
| 1    | > 400 cm    | Silent              |
| 2    | 301–400 cm  | Slow beep (800ms)   |
| 3    | 201–300 cm  | Medium beep (400ms) |
| 4    | 51–200 cm   | Fast beep (150ms)   |
| 5    | 0–50 cm     | Continuous ON       |

---

## Hardware

| Component | Details |
|-----------|---------|
| Microcontroller | LPC2129 (ARM7TDMI-S) — Rhydolabz Board |
| CAN Transceiver | MCP2551 or SN65HVD230 |
| Ultrasonic Sensor | HC-SR04 |
| Buzzer | Active buzzer (P0.21 — Node A) |
| Reverse Switch | Push button on P0.16 (EINT0 — Node A) |
| Debug | UART0 → USB-to-TTL → Serial terminal |

---

## Pin Connections

### Node A
| Pin  | Function          |
|------|-------------------|
| P0.0 | UART0 TXD         |
| P0.1 | UART0 RXD         |
| P0.16| EINT0 (Gear SW)   |
| P0.18| LED indicator     |
| P0.21| Buzzer output     |
| P0.25| CAN1 RD1 (RX)     |

### Node B
| Pin  | Function               |
|------|------------------------|
| P0.0 | UART0 TXD              |
| P0.1 | UART0 RXD              |
| P0.10| ECHO input (HC-SR04)   |
| P0.11| TRIG output (HC-SR04)  |
| P0.25| CAN1 RD1 (RX)          |

---

## Project Structure

```
CAN_Project/
│
├── NodeA/
│   ├── nodeA_main.c          # Entry point: gear detection, CAN TX/RX, buzzer
│   ├── nodeA_can_driver.c    # CAN1 init, blocking TX and RX
│   ├── nodeA_uart_driver.c   # UART0 init, TX, RX, print functions
│   ├── nodeA_sw_interrupt.c  # EINT0 ISR: reverse gear toggle
│   ├── nodeA_timer_intr.c    # Timer1 ISR: adaptive buzzer control
│   ├── delay.c               # Timer0 blocking delays (ms, us, sec)
│   ├── header.h              # Typedefs, structs, pin defines, declarations
│   └── Startup.s             # LPC2129 ARM startup file (from Keil)
│
└── NodeB/
    ├── nodeB_main.c          # Entry point: wait for RTR, measure, respond
    ├── nodeB_can_driver.c    # CAN1 init with RX interrupt, TX
    ├── nodeB_uart_driver.c   # UART0 init, TX, RX, print functions
    ├── nodeB_receive_intr.c  # CAN1 RX ISR: captures RTR frame ID and DLC
    ├── nodeB_ultrasonic.c    # HC-SR04 distance measurement with timeout
    ├── delay.c               # Timer0 blocking delays (ms, us, sec)
    └── header.h              # Typedefs, structs, pin defines, declarations
```

---

## How to Build in Keil uVision

1. Open Keil uVision → **New Project**
2. Select device: **NXP LPC2129**
3. Add files for the node you are building (NodeA or NodeB):
   - Add all `.c` files from the respective folder
   - Add `Startup.s`
4. In **Target Options**:
   - Set XTAL to `12 MHz`
   - Set **Optimization**: Level 1
5. Build → Flash using JTAG or ISP

> Build NodeA and NodeB as **separate Keil projects** — they run on two different LPC2129 boards.

---

## Serial Monitor Output

### Node A
```
Node A Ready: CAN1 & UART0 Initialized
Reverse Gear Engaged
Distance: 312 cm
Distance: 278 cm
Distance: 145 cm
Distance: 43 cm
```

### Node B
```
Node B Ready: CAN1 & UART0 Initialized
Distance: 312 cm
Distance: 278 cm
Distance: 145 cm
Distance: 43 cm
```

---

## Key Concepts Demonstrated

- ARM7 register-level embedded C programming (no HAL/BSP)
- CAN Bus protocol — RTR frame, data frame, message ID filtering
- Interrupt-driven design — EINT0, Timer1, CAN RX ISR
- Asynchronous event handling between two independent nodes
- Ultrasonic sensor interfacing with pulse-width timing
- UART driver implementation from scratch
- Hardware-software co-design on LPC2129

---

## Tools Used

| Tool | Purpose |
|------|---------|
| Keil uVision | IDE and ARM compiler |
| LPC2129  | Target hardware |
| Hercules / PuTTY | Serial monitor for UART debug |
| CAN Analyzer (optional) | Monitor CAN bus frames |
+-
