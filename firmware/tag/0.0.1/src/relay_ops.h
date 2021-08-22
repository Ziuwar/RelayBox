#ifndef RELAY_OPS_H
#define RELAY_OPS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool relay_control(const char *const relay,const char *const setting);
bool read_relay_reg(void);

#ifdef __cplusplus
}
#endif

#endif
