#include "command_exec.h"
#include "USART0_ops.h"
#include "save_config.h"
#include "relay_ops.h"
#include "eeprom_ops.h"
#include "man_page.h"

#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_COMMAND_LEN 24U
volatile char raw_command[MAX_COMMAND_LEN];		// Stores commands incoming from the USART0

// Command evaluation and execution
bool command_execute(const volatile char *const command)
{
    if((char)command[0] == 'R')	// Relay commands
    {
        char c_relay = command[1];
        char c_setting = command[3];

        if((c_relay == '1' || c_relay == '2' || c_relay == '3' || c_relay == '4' || c_relay == '5' || c_relay == '6' || c_relay == '7' || c_relay == 'A') && (c_setting == '0' || c_setting == '1') && (command[2] == '_') && (strlen(command) <= 4))
        {
            relay_control(&c_relay, &c_setting);
        }
        else
        {
            USART0_send_string("Wrong relay (1-7), or setting (0,1). Example: R1_1 (Relay 1 = ON)\r\n");
            return true;
        }
    }
    else if(command[0] == 'S') // Read actual status
    {
        if(strlen(command) <= 1)
        {
            read_relay_reg();
        }
        else
        {
            USART0_send_string("Wrong status command. Example: S\r\n");
            return true;
        }
    }
    else if(command[0] == 'C')	// Save configuration commands -> CS_NAME_123 , CR , CL_NAME , CD_NAME
    {
        if(strlen(command) <= 24)
        {
            config_management(command);
        }
        else
        {
            USART0_send_string("Wrong configuration command. Example: C\r\n");
            return true;
        }

        raw_command[0] = '\0';	// Reset global variable

    }
    else if (command[0] == 'H')	// Help command -> Shows the man page
    {
        man_page();
    }
    else if (command[0] == 'E' && command[1] == 'C')	// Help command -> Shows the man page
    {
        clear_eeprom();
    }
    else // No valid command found
    {
        raw_command[0] = '\0';	// Reset global variable
        USART0_send_string("Wrong command!\r\n");
    }
    return false;
}

#ifdef __cplusplus
}
#endif
