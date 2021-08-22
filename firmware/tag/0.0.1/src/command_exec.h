#ifndef COMMAND_EXEC_H
#define COMMAND_EXEC_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool command_execute(const volatile char *const command);

#ifdef __cplusplus
}
#endif

#endif
