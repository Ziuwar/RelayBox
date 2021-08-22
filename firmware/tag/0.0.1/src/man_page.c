#include "man_page.h"
#include "USART0_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

//MAN page
void man_page(void)
{
    USART0_send_string("Seven relays (1-7) are available. Commands implemented:\r\nR1_1 -> Relay1_ON | R3_0 -> Relay3_OFF\r\nRA_1 -> All relays ON | RA_0 -> All relays OFF\r\nS -> Status of the relays\r\n");
    USART0_send_string("CS_Name12_134 -> Configuration save \r\n                 max. 13 ASCII for name, relay(s) to be enabled (ON)\r\nCR -> Read configuration(s)\r\n");
    USART0_send_string("CL_Name12 -> Configuration load\r\nCD_Name12 -> Configuration delete\r\nH -> Show this man page\r\nEC -> Clears the EEPROM. Reboot needed.\r\nNOTE: Underscore is used as a command separator. Not usable in any other way.\r\n");
}

#ifdef __cplusplus
}
#endif
