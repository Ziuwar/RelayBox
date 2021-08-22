#include "eeprom_ops.h"
#include "USART0_ops.h"

#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/eeprom.h>

#ifdef __cplusplus
extern "C" {
#endif

// Converts the user input for relays into a hex form for storage in the eeprom
uint8_t char_to_byte (const volatile char *const conf_relays)
{
    uint8_t relays_byte = 0x00U;

    for (uint8_t i = 0x00U; i < strlen(conf_relays); i++)
    {
        if(conf_relays[i] == '1')
        {
            relays_byte |= (1<<0);
        }
        else if (conf_relays[i] == '2')
        {
            relays_byte |= (1<<1);
        }
        else if (conf_relays[i] == '3')
        {
            relays_byte |= (1<<2);
        }
        else if (conf_relays[i] == '4')
        {
            relays_byte |= (1<<3);
        }
        else if (conf_relays[i] == '5')
        {
            relays_byte |= (1<<4);
        }
        else if (conf_relays[i] == '6')
        {
            relays_byte |= (1<<5);
        }
        else if (conf_relays[i] == '7')
        {
            relays_byte |= (1<<6);
        }
    }

    return relays_byte;
}

// Find a free 16 byte address room in the eeprom
uint8_t find_start_eeprom(void)
{
    uint8_t start_byte = 0x00U;

    for (uint16_t row_start = 0; row_start <= 0xf0U; row_start = row_start + 0x10U)	// Loops through the 16 pages available for saving a config
    {
        start_byte = eeprom_read_byte((uint8_t*)row_start);

        if(start_byte == 0xffU)	// A space is consider free if the first byte is ff
        {
            return (row_start);	// Returns the start position of the first free eeprom space found
        }
    }
    return 0xffU;				// Returning 265 means, no free space found
}

// Write EEPROM config
void write_config(const volatile char *const conf_name,  uint8_t relays_byte,  uint8_t start_address)
{
    volatile uint8_t count_byte = 0x00U;
    volatile bool end_flag = false;

    count_byte = start_address;

    cli();	// Disable global interrupts before EEPROM write access

    for(uint8_t i = 0; i <= 15; ++i)
    {
        if ((end_flag == false) && ((count_byte - start_address) <= 12)) // Byte 0 - 12 beginning from the start byte is the name
        {
            if(conf_name[(count_byte-start_address)] == '\0')
            {
                end_flag = true;
            }
            else
            {
                eeprom_write_byte((uint8_t*)count_byte, (char )conf_name[(count_byte-start_address)]);
            }
        }
        else if ((end_flag = true) && ((count_byte - start_address) <= 12)) // After the '\0' is seen, the remaining bytes are filled with 0xff
        {
            eeprom_write_byte((uint8_t*)count_byte, (char ) 0xffU);
        }
        else if((count_byte - start_address) == 13) // Byte 13 from the start byte is the relay config
        {
            eeprom_write_byte((uint8_t*)count_byte, (char )relays_byte);
        }
        else
        {
            eeprom_write_byte((uint8_t*)count_byte, (char ) 0xffU); // Byte 14 and 15 are filled with 0xff
        }
        ++count_byte;
    }

    sei();	// Enable global interrupts after EEPROM write access
}

// DANGER !!
void clear_eeprom(void)
{
    cli();	// Disable global interrupts before EEPROM write access

    for(uint8_t i = 0; i <= 0xfeU; ++i)
    {
        if(eeprom_read_byte((uint8_t*)i) != 0xffU)
        {
            eeprom_write_byte((uint8_t*)i, (char )0xffU);
        }
    }

    sei();	// Enable global interrupts after EEPROM write access

    USART0_send_string("EEPROM deleted\r\n");
}

// Reads the relay configuration in the given row and converts it from binary to char for UART0 output
uint8_t read_relays(uint16_t save_row)
{
    volatile uint8_t relay_config_byte = 0x00U;
    volatile char relay_config_char[7];
    volatile uint8_t char_count = 0;
    volatile uint8_t relay = 49;

    relay_config_byte = eeprom_read_byte((uint8_t*)(save_row+13));		// Location of the relay config is byte 13 of the page

    for(uint8_t mask = 0x01U; mask <= 0x40U; mask = mask<<1)			// Loops through the 7 relay bit positions
    {
        if((relay_config_byte & mask) && 0x01U)				// Checks if the relay is set to 1 -> enabled
        {
            relay_config_char[char_count] = (char)relay;	// Appends the found relay to the string
            ++char_count;									// Is used to fill the string, is incremented when a relay is enabled
        }
        ++relay;											// Counts from 1 to 7, corresponds to the relay mask, ASCII 49 - 55
    }

    relay_config_char[char_count] = '\0';					// Appends the string termination

    USART0_send_string(relay_config_char);
    USART0_send_string("\r\n");

    return relay_config_byte;
}

// Reads the configuration name in the given row and converts it to a string for UART0 output
void read_names(uint16_t save_row)
{
    volatile char save_name[13];
    volatile uint8_t character = 0x00U;
    volatile uint8_t char_count = 0x00U;

    for(; eeprom_read_byte((uint8_t*)(save_row+char_count)) != 0xffU && char_count <= 12; ++char_count)				// Loops through the bytes of one page
    {
        character = eeprom_read_byte((uint8_t*)(save_row+char_count));
        save_name[char_count] = character;
    }

    save_name[char_count] = '\0';
    char_count = 0x00U;

    USART0_send_string(save_name);
}

// Find and execute saved configuration
void set_relay_config(const char *command)
{
    char save_name_input[14];
    char save_name[14];
    uint8_t relay_byte = 0x00U;
    uint8_t character = 0x00U;
    uint8_t char_count = 0x00U;

    for(uint8_t count = 3; count <= strlen(command); ++count)		// Truncate the first 3 chars (CL_), so only the config name remains
    {
        save_name_input[(count-3)] = command[count];
    }

    for (uint16_t row_start = 0; row_start < 256; row_start = row_start + 0x10U)					// Loops through the 16 pages available for saving a config
    {
        if(eeprom_read_byte((uint8_t*) row_start) != 0xffU)											// Checks if the page holds a saved configuration
        {
            for(; eeprom_read_byte((uint8_t*)(row_start+char_count)) != 0xffU && char_count <= 12; ++char_count)		// Loops through the bytes of one page
            {
                character = eeprom_read_byte((uint8_t*)(row_start+char_count));
                save_name[char_count] = character;
            }
            save_name[(char_count)] = '\0';

            if(strcmp(save_name, save_name_input) == 0)	// Compare the user input with the save names in the eeprom
            {
                cli();	// Disable global interrupts before PORTA write access

                PORTA.OUT &= ~0xffU;					// Clear the register
                relay_byte = read_relays(row_start);
                PORTA.OUT |= relay_byte << 1;			// Set the register to the value from the eeprom

                sei();	// Enable global interrupts after PORTA write access

                return;	// Get back after finding a valid entry and ignore the rest
            }
            char_count = 0x00U;
        }
    }
    USART0_send_string("Saved configuration not found.\r\n");
}

// Delete saved configuration
void delete_relay_config(const char *command)
{
    char save_name_input[14];
    char save_name[14];
    uint8_t character = 0x00U;
    uint8_t char_count = 0x00U;

    for(uint8_t count = 3; count <= strlen(command); ++count)		// Truncate the first 3 chars (CL_), so only the config name remains
    {
        save_name_input[(count-3)] = command[count];
    }

    for (uint16_t row_start = 0; row_start < 256; row_start = row_start + 0x10U)					// Loops through the 16 pages available for saving a config
    {
        if(eeprom_read_byte((uint8_t*) row_start) != 0xffU)											// Checks if the page holds a saved configuration
        {
            for(; eeprom_read_byte((uint8_t*)(row_start+char_count)) != 0xffU && char_count <= 12; ++char_count)		// Loops through the bytes of one page
            {
                character = eeprom_read_byte((uint8_t*)(row_start+char_count));
                save_name[char_count] = character;
            }
            save_name[char_count] = '\0';

            if(strcmp(save_name, save_name_input) == 0)	// Compare the user input with the save names in the eeprom
            {
                cli();	// Disable global interrupts before EEPROM write access

                for(uint8_t byte_count = 0x00U; byte_count <= 0x0eU; ++byte_count)
                {
                    eeprom_write_byte((uint8_t*)(row_start+byte_count),0xffU);
                }

                sei();	// Enable global interrupts after EEPROM write access

                USART0_send_string("Saved configuration deleted.\r\n");

                return;	// Get back after finding a valid entry and ignore the rest
            }
            char_count = 0x00U;
        }
    }
    USART0_send_string("Saved configuration not found.\r\n");
}


#ifdef __cplusplus
}
#endif
