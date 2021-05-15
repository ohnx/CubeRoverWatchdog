/**
 * @file uart.h
 * @brief File for interfacing with UART protocol hardware module
 * File for interfacing with UART protocol hardware module. Also includes some
 * basic communications protocol parsing.
 * 
 * UART communication uses eUSCI_A0 (Hercules) and eUSCI_A1 (Lander).
 */
#ifndef __UART_INC
#define __UART_INC

#include "buffer.h"

/**
 * @brief Initialize UART hardware.
 * Sets up the interrupts and whatnot for UART.
 */
void uart_init();

/**
 * @brief Send SLIP-encoded bytes via UART 1
 * Sends SLIP-encoded bytes to the lander using UART 1
 */
void uart1_tx_nonblocking(uint16_t length, unsigned char *buffer);

#endif /* __UART_INC */
