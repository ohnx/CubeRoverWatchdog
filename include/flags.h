/**
 * @file flags.h
 * @brief File outlining the flags that are used to communicate with the main loop
 */
#ifndef __FLAGS_INC
#define __FLAGS_INC

#include <stdint.h>

#define FLAG_UART0_RX_PACKET 0x1
#define FLAG_UART1_RX_PACKET 0x2
#define FLAG_I2C_RX_PACKET   0x4
#define FLAG_BUTTON_PRESSED  0x8

__volatile extern uint16_t loop_flags;



#endif /* __FLAGS_INC */
