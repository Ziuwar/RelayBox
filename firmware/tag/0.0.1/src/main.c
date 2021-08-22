/*
 * main.c
 *
 * Created: 09.06.2017 14:38:52
 * Author : Andreas.Schroeder
 */

#include "init.h"
#include "eeprom_ops.h"
#include "USART0_ops.h"
#include "man_page.h"
#include "relay_ops.h"
#include "save_config.h"
#include "command_exec.h"

// Defines and macros
#define MAX_COMMAND_LEN 24U

// Includes
#include <avr/io.h>
#include <avr/interrupt.h>

// Global variables
volatile char raw_command[MAX_COMMAND_LEN];		// Stores commands incoming from the USART0
volatile uint8_t i_index = 0U;					// Used as a pointer to assemble the command string
volatile bool b_rx_done = false;				// Is set when a command is received and reset when the command is processed

// ISR for UART0 receive complete
ISR(USART0_RXC_vect)
{

    const volatile uint8_t i_incoming = USART0_RXDATAL;		// Read the incoming byte.

    if(!b_rx_done)		// Check if the last received command was processed.
    {

        if((i_incoming != '\n') && (i_incoming != '\r'))		// Assemble the command string. \n is the end character. Max 24 digits.
        {
            raw_command[i_index++] = i_incoming;

            if(i_index > MAX_COMMAND_LEN)
            {
                USART0_send_string("Max length 24 digits.\r\n");
                i_index = 0;
                raw_command[0] = '\0';
                b_rx_done = false;
            }
        }

        else if(i_incoming == '\n')		// \n adds the termination character to the string.
        {
            raw_command[i_index] = '\0';
            i_index = 0;
            b_rx_done = true;			// This global flag is polled in the main() and reset after a command is processed.
        }
    }
}

// Real time counter interrupt vector
ISR(RTC_CNT_vect)
{
    PORTB.OUTTGL = PIN0_bm;				// Toggles LED1
    RTC.INTFLAGS |= 0x02;				// Reset the RTC counter interrupt
    while(!(RTC.STATUS == 0x00)) { }	// Wait for the RTC to be ready for config
    RTC.CNT	&= ~0xFFFF;					// Sets the counter back to zero
}

// Main
int main(void)
{
    // System initialization
    port_config();			// IO config
    USART0_init();			// USART0 Initialization
    RTC_config();			// Timer RTC configuration
    interrupt_init();		// Initialization of the interrupts

    USART0_send_string("*** Send H for help. EOL must be CR followed by LF ***\r\n");

    // Main loop
    while(1)
    {
        if(b_rx_done)							// Trigger the command execution when the rx done flag is set
        {
            USART0_send_string("**Command: ");
            USART0_send_string(raw_command);	// Print command to serial
            USART0_send_string("**\r\n");
            command_execute(raw_command);		// Call command evaluation and execution
            raw_command[0] = '\0';				// Erase the command array
            b_rx_done = false;					// Reset the command received flag to enable the receiver
        }
    }
    return 0;
}