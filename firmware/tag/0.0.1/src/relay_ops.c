#include "relay_ops.h"
#include "USART0_ops.h"

#include <avr/io.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Identifies the relay affected and sets desired state
bool relay_control(const char *const relay,const char *const setting)
{
    if (relay && setting)	// Null pointer check --> What action to take??
    {
        switch(*setting)
        {
            case '1':			// Setting is ON
                switch(*relay)	// Enable the output pin of the identified relay
                {
                    case '1':
                        PORTA.OUT |= PIN1_bm;
                        USART0_send_string("Relay 1 ON\r\n");
                        break;
                    case '2':
                        PORTA.OUT |= PIN2_bm;
                        USART0_send_string("Relay 2 ON\r\n");
                        break;
                    case '3':
                        PORTA.OUT |= PIN3_bm;
                        USART0_send_string("Relay 3 ON\r\n");
                        break;
                    case '4':
                        PORTA.OUT |= PIN4_bm;
                        USART0_send_string("Relay 4 ON\r\n");
                        break;
                    case '5':
                        PORTA.OUT |= PIN5_bm;
                        USART0_send_string("Relay 5 ON\r\n");
                        break;
                    case '6':
                        PORTA.OUT |= PIN6_bm;
                        USART0_send_string("Relay 6 ON\r\n");
                        break;
                    case '7':
                        PORTA.OUT |= PIN7_bm;
                        USART0_send_string("Relay 7 ON\r\n");
                        break;
                    case 'A':
                        PORTA.OUT |= 0xFE;
                        USART0_send_string("All relays ON\r\n");
                        break;
                }
                break;

            case '0':	// Setting is OFF

                switch(*relay)	// Disable the output pin of the identified relay
                {
                    case '1':
                        PORTA.OUT &= ~PIN1_bm;
                        USART0_send_string("Relay 1 OFF\r\n");
                        break;
                    case '2':
                        PORTA.OUT &= ~PIN2_bm;
                        USART0_send_string("Relay 2 OFF\r\n");
                        break;
                    case '3':
                        PORTA.OUT &= ~PIN3_bm;
                        USART0_send_string("Relay 3 OFF\r\n");
                        break;
                    case '4':
                        PORTA.OUT &= ~PIN4_bm;
                        USART0_send_string("Relay 4 OFF\r\n");
                        break;
                    case '5':
                        PORTA.OUT &= ~PIN5_bm;
                        USART0_send_string("Relay 5 OFF\r\n");
                        break;
                    case '6':
                        PORTA.OUT &= ~PIN6_bm;
                        USART0_send_string("Relay 6 OFF\r\n");
                        break;
                    case '7':
                        PORTA.OUT &= ~PIN7_bm;
                        USART0_send_string("Relay 7 OFF\r\n");
                        break;
                    case 'A':
                        PORTA.OUT &= ~0xFE;
                        USART0_send_string("All relays OFF\r\n");
                        break;
                }
                break;
        }
        return false;
    }
    else
    {
        return true;
    }
}

// Read status of relay 1 to relay 7 and send over USART0
bool read_relay_reg(void)
{
    bool relay_on_found = false;

    if((PORTA.OUT & PIN1_bm) && 0x02)
    {
        USART0_send_string("Relay 1: ON\r\n");
        relay_on_found = true;
    }
    if ((PORTA.OUT & PIN2_bm) && 0x04)
    {
        USART0_send_string("Relay 2: ON\r\n");
        relay_on_found = true;
    }
    if ((PORTA.OUT & PIN3_bm) && 0x06)
    {
        USART0_send_string("Relay 3: ON\r\n");
        relay_on_found = true;
    }
    if ((PORTA.OUT & PIN4_bm) && 0x08)
    {
        USART0_send_string("Relay 4: ON\r\n");
        relay_on_found = true;
    }
    if ((PORTA.OUT & PIN5_bm) && 0x10)
    {
        USART0_send_string("Relay 5: ON\r\n");
        relay_on_found = true;
    }
    if ((PORTA.OUT & PIN6_bm) && 0x20)
    {
        USART0_send_string("Relay 6: ON\r\n");
        relay_on_found = true;
    }
    if ((PORTA.OUT & PIN7_bm) && 0x40)
    {
        USART0_send_string("Relay 7: ON\r\n");
        relay_on_found = true;
    }
    if(!relay_on_found)
    {
        USART0_send_string("All relays: OFF\r\n");
    }
    return false;
}

#ifdef __cplusplus
}
#endif
