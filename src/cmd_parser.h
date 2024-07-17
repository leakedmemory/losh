#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    char **args;
    size_t size;
    char *input_file;
    char *output_file;
    bool append;

    size_t _capacity;
} Command;

void cmd_init(Command *cmd);
void cmd_free(Command *cmd);
void cmd_clean(Command *cmd);

int8_t cmd_parse_input(Command *cmd, char *input);
