#include "save_config.h"
#include "eeprom_ops.h"
#include "USART0_ops.h"

#include <avr/eeprom.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Sub-menu for managing stored configurations
void config_management(const volatile char *const command)
{

    volatile uint8_t i_pos = 3;			// Start at position 3, to step over the CS_ command
    volatile char conf_name[14];		// Stores the config name - Max 13 chars
    volatile char conf_relays[7];		// Stores the config name - Max 7 chars
    volatile uint8_t relay_pos = 0U;
    volatile uint8_t name_pos = 0U;


    if((command[1] == 'S') && (command[2] == '_'))		// Save config
    {

        conf_name[0] = '\0';
        conf_relays[0] = '\0';

        USART0_send_string("Safe config command!\r\n");

        // 1. Search for the second '_' after the first '_' -> This gives the save name
        for(; (command[i_pos] != '_'); i_pos++)
        {
            if(command[i_pos] == '\0')
            {
                USART0_send_string("Safe config command wrong! Second '_' not found.\r\n");
                return;
            }
            //else if (i_pos >= 16)
            //{
            //USART0_send_string("Safe config command wrong! Name to long (max. 13 letters).\r\n");
            //return;
            //}
            else
            {
                conf_name[name_pos] = command[i_pos];
                name_pos++;
            }
        }

        // 2. Append a string end and step over the second '_'
        conf_name[name_pos] = '\0';
        i_pos++;

        // 3. Extracted the desired relays to be set -> After second '_' to '\0'
        for(; command[i_pos] != '\0'; i_pos++)
        {
            if(command[i_pos] == '1' || command[i_pos] == '2' || command[i_pos] == '3' || command[i_pos] == '4' || command[i_pos] == '5' || command[i_pos] == '6' || command[i_pos] == '7') // Only relays 1 to 7 allowed
            {
                conf_relays[relay_pos] = command[i_pos];
                relay_pos++;
            }
            else
            {
                USART0_send_string("Safe config command wrong! One or more relays not found.\r\n");
                return;
            }
        }

        // 4. Append a string end -> NOT for EEPROM writing, only testing
        conf_relays[relay_pos] = '\0';

        // 5. Console output for verification by the user
        USART0_send_string("Name: ");
        USART0_send_string(conf_name);
        USART0_send_string("\r\n");
        USART0_send_string("Relays ON: ");
        USART0_send_string(conf_relays);
        USART0_send_string("\r\n");

        // 6. Convert the conf_relay char command into a 0x00 form
        uint8_t relays_byte = char_to_byte(conf_relays);

        // 7. Search for a free address space (16 byte) in the eeprom
        uint8_t start_address = find_start_eeprom();

        if(start_address != 0xffU)
        {
            // 8. Write the save config in the eeprom
            write_config(conf_name, relays_byte, start_address);
        }
        else
        {
            USART0_send_string("No free page in the eeprom found!");
            return;
        }
    }
    else if((command[1] == 'R') && (command[2] == '\0'))			// Read config
    {

        uint8_t found_one = 0x00U;

        USART0_send_string("Read configurations command!\r\n"); // Reads all saved configurations, writes them to UART in format NAME_1234567

        for (uint16_t row_start = 0; row_start < 256; row_start = row_start + 0x10U)	// Loops through the 16 pages available for saving a config
        {
            if(eeprom_read_byte((uint8_t*) row_start) != 0xffU)							// Checks if the page holds a saved configuration
            {
                read_names(row_start);
                USART0_send_string("_");
                read_relays(row_start);
                ++found_one;
            }
        }
        if (found_one == 0x00U)
        {
            USART0_send_string("No configuration(s) found!\r\n");
        }
    }

    else if ((command[1] == 'L') && (command[2] == '_'))			// Load Config
    {
        USART0_send_string("Load config command!\r\n");
        set_relay_config(command);
    }

    else if ((command [1] == 'D') && (command[2] == '_'))			// Delete config
    {

        USART0_send_string("Delete config command!\r\n");
        delete_relay_config(command);

    }
    else
    {
        USART0_send_string("Wrong configuration command!\r\n");
        return;
    }

    return;
}

#ifdef __cplusplus
}
#endif
