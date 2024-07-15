#pragma once

#include <stdint.h>

#include "cmd_parser.h"

typedef int32_t (*ExecFunction)(char **args);

void cmd_exec(Command *cmd);
