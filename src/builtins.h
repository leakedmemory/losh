#pragma once

#include "cmd_exec.h"

typedef struct {
    ExecFunction func;
    uint32_t min_args;
    uint32_t max_args;
} BuiltinCmd;

void builtins_init(void);
void builtins_free(void);
BuiltinCmd *get_builtin(const char *mnemonic);
