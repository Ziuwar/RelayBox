#ifndef EEPROM_OPS_H
#define EEPROM_OPS_H

#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t char_to_byte (const volatile char *const conf_relays);
uint8_t find_start_eeprom(void);
void write_config(const volatile char *const conf_name,  uint8_t relays_byte,  uint8_t start_address);
void clear_eeprom(void);
uint8_t read_relays(uint16_t save_row);
void read_names(uint16_t save_row);
void set_relay_config(const char *command);
void delete_relay_config(const char *command);

#ifdef __cplusplus
}
#endif

#endif
