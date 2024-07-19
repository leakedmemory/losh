#pragma once

#include <stddef.h>
#include <stdint.h>

#define CMD_HISTORY_PATH "~/.losh_history"

int8_t save_cmd_in_history(const char *cmd, size_t size, const char *filepath);
int8_t print_cmd_history(const char *filepath);
