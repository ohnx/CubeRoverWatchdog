#include <msp430.h>
#include "include/buffer.h"
#include "include/uart.h"
#include "include/flags.h"

/* define all of the global variables used in other files */
__volatile struct buffer uart0rx, uart0tx, uart1rx, uart1tx, i2crx, i2ctx;
__volatile uint16_t loop_flags;

/**
 * main.c
 */
int main(void) {
    /* stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;

	/* unlock changes to registers/ports, etc. */
	PM5CTL0 &= ~LOCKLPM5;

    /* set up uart */
    uart_init();

    // setup on-board LEDs
    P1OUT &= 0x00;               // Shut down everything
    P1DIR &= 0x00;
    P1DIR |= BIT0;       // P1.0 and P1.1 pins output, the rest are input
    P1DIR |= BIT1;
    P1DIR |= BIT2;

    // setup buttons
    P5REN |= BIT6;                 // Enable internal pull-up/down resistors
    P5OUT |= BIT6;                 //Select pull-up mode for P5.6
    P5REN |= BIT5;                 // Enable internal pull-up/down resistors
    P5OUT |= BIT5;                 //Select pull-up mode for P5.5

    P5IE |= BIT6;                    // P5.6 interrupt enabled
    P5IES |= BIT6;                  // P5.6 Hi/lo edge
    P5IFG &= ~BIT6;               // P5.6 IFG cleared

    P5IE |= BIT5;                    // P5.5 interrupt enabled
    P5IES |= BIT5;                  // P5.5 Hi/lo edge
    P5IFG &= ~BIT5;               // P5.5 IFG cleared

    // setup timer
//    TA0CCTL0 = CCIE;                             // CCR0 interrupt enabled
//    TA0CTL = TASSEL_2 + MC_1 + ID_3;           // SMCLK/8, upmode
//    TA0CCR0 = 65535;

    while (1) {
        /* check if anything happened */
        __no_operation();

        if (!loop_flags) { /* nothing did */
            /* set the status register to be in LPM0 */
            __bis_SR_register(GIE);//LPM0_bits + GIE);
            continue;
        }

        /* a cool thing happened! now time to check what it was */
        if (loop_flags & FLAG_UART0_RX_PACKET) {
            /* clear event when done */
            loop_flags ^= FLAG_UART0_RX_PACKET;
        }
        if (loop_flags & FLAG_UART1_RX_PACKET) {
            /* clear event when done */
            loop_flags ^= FLAG_UART1_RX_PACKET;
            P1OUT ^= BIT1;
        }
        if (loop_flags & FLAG_I2C_RX_PACKET) {
            /* clear event when done */
            loop_flags ^= FLAG_I2C_RX_PACKET;
        }
        if (loop_flags & FLAG_BUTTON_PRESSED) {
            /* debug: send a message! */
            uart1_tx_nonblocking(5, "Hi!\r\n");
            /* toggle led */
            //P1OUT ^= BIT0;
            /* clear event when done */
            loop_flags ^= FLAG_BUTTON_PRESSED;
        }
    }

    /* hopefully we will never reach here... */
	return 0;
}


// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    P1OUT ^= BIT0;
}


// Port 5 interrupt service routine
// maximum queue of 1 interrupt for each thing
#pragma vector=PORT5_VECTOR
__interrupt void Port_5(void)
{
    if (P5IFG & BIT5) {
        // toggle the LED
        P5IFG &= ~(BIT5);
        loop_flags |= FLAG_BUTTON_PRESSED;
    }
    if (P5IFG & BIT6) {
        // toggle the LED
        P5IFG &= ~(BIT6);
        loop_flags |= FLAG_BUTTON_PRESSED;
    }
}

