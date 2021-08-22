#include "init.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 3333333U // The CPU speed -> 20 MHz / 6 = 3.33 MHz
#define USART0_BAUD_RATE(BAUD_RATE) ((float) (F_CPU * 64U / (16U * (float)BAUD_RATE)) + 0.5f) // Calculates the register setting for the given CPU speed and baud rate

#ifdef __cplusplus
extern "C" {
#endif

// Initialization of the interrupts
void interrupt_init(void)
{
    USART0.CTRLA |= USART_RXCIE_bm;		 	// Enables the receive interrupt complete vector for UART0
    RTC.INTCTRL  |= RTC_CMP_bm;				// Enables the RTC compare interrupt
    sei();									// Enable global interrupts
}

// IO config
void port_config(void)
{
    // Outputs
    PORTA.DIR |= PIN1_bm;	// Relay_1
    PORTA.DIR |= PIN2_bm;	// Relay_2
    PORTA.DIR |= PIN3_bm;	// Relay_3
    PORTA.DIR |= PIN4_bm;	// Relay_4
    PORTA.DIR |= PIN5_bm;	// Relay_5
    PORTA.DIR |= PIN6_bm;	// Relay_6
    PORTA.DIR |= PIN7_bm;	// Relay_7

    PORTB.DIR |= PIN0_bm;	// LED LD1
    PORTB.PIN0CTRL |= 0x80;	// Invert bit
    PORTB.DIR |= PIN1_bm;	// LED LD2
    PORTB.PIN1CTRL |= 0x80; // Invert bit
}

// USART0 Initialization
void USART0_init(void)
{
    PORTB.DIR &= ~PIN3_bm; // UART RX -> Input
    PORTB.DIR |= PIN2_bm;  // UART TX -> Output

    USART0.BAUD = (uint16_t) USART0_BAUD_RATE(9600);	// Set the register to baud rate 9600 using the defined macro
    USART0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;			// Set the tx and rx enable to 1 (enabled)
}

// Timer RTC configuration
void RTC_config(void)
{
    RTC_CLKSEL	|= RTC_CLKSEL_INT1K_gc;		// Set clock source to OSCULP32K (1,024 kHz)
    while(!(RTC.STATUS == 0x00U)) { }		// Wait for the RTC to be ready for config
    RTC.CTRLA	|= RTC_PRESCALER_DIV1_gc;	// Set prescaler to DIV1, results in a timer with 1,024 ms
    RTC.CMP		|= 0x01E9U;					// Sets the compare to 489, resulting in 500,736 ms time slots
    while(!(RTC.STATUS == 0x00U)) { }		// Wait for the RTC to be ready for config
    RTC.CTRLA	|= 0x1U;					// Enable the RTC
}







#ifdef __cplusplus
}
#endif
