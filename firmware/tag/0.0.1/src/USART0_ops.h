#ifndef USART0_OPS_H
#define USART0_OPS_H

#ifdef __cplusplus
extern "C" {
#endif

void USART0_send_char(const char c);
void USART0_send_string(const volatile char *const str);

#ifdef __cplusplus
}
#endif

#endif
