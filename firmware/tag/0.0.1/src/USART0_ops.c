#include "USART0_ops.h"

#include <avr/io.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Send a char with USART0
void USART0_send_char(const char c)
{
    while(!(USART0.STATUS & USART_DREIF_bm)) { }
    USART0.TXDATAL = c;
}

// Send a string with USART0
void USART0_send_string(const volatile char *const str)
{
    if(*str)		// Null pointer check --> What action to take??
    {
        for(uint32_t i = 0; i < strlen(str); i++)
        {
            USART0_send_char(str[i]);
        }
    }
}

#ifdef __cplusplus
}
#endif
