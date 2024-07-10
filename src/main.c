#include "cmd_exec.h"
#include "cmd_parser.h"
#include "env.h"
#include "io_handler.h"

#define INPUT_SIZE 4096

int main(void) {
    env_init();
    io_alloc_cfg_singleton(stdout, stdin, stderr);

    char input[INPUT_SIZE];
    Command cmd;
    cmd_init(&cmd);

    while (1) {
        io_write("losh$ ");
        io_flush_out_stream();

        if (io_read(input, INPUT_SIZE) < 0) {
            io_perror("ERROR: Input reading failed");
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
    }
}
