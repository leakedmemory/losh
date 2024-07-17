#include <stdlib.h>

#include "builtins.h"
#include "cmd_exec.h"
#include "cmd_parser.h"
#include "env.h"
#include "io_handler.h"
#include "prompt.h"
#include "signal_handler.h"

#define MAX_INPUT_SIZE 4096

int main(void) {
    env_init();
    builtins_init();
    set_signal_handlers();
    io_alloc_cfg_singleton(stdin, stdout, stderr);

    char input[MAX_INPUT_SIZE];
    Command cmd;
    cmd_init(&cmd);

    while (1) {
        write_prompt();

        int32_t bytes_read = io_read(input, MAX_INPUT_SIZE);
        // ctrl + d
        if (bytes_read == EOF) {
            io_write("\n");
            exit(EXIT_FAILURE);
        } else if (bytes_read <= 0) {
            io_write_err("ERROR: Input reading failed\n");
            continue;
        }

        if (input[0] == '\n') {
            continue;
        }

        if (cmd_parse_input(&cmd, input) == 0) {
            cmd_exec(&cmd);
            cmd_clean(&cmd);
        } else {
            io_perror("Input parsing failed");
        }

        io_write("\n");
    }
}
