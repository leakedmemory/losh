#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
    char **_args;
    size_t _size;
    size_t _capacity;
} Command;

void cmd_init(Command *cmd);
void cmd_free(Command *cmd);
void cmd_clean(Command *cmd);

int8_t cmd_parse_input(Command *cmd, char *input);

const char *cmd_get_mnemonic(Command *cmd);
char *const *cmd_get_args(Command *cmd);
