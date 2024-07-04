#include "cmd_parser.h"
#include "io_handler.h"

#define INPUT_SIZE 4096

int main(void) {
    char input[INPUT_SIZE];
    io_alloc_cfg_singleton(stdout, stdin, stderr);
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
            // TODO: execute command
            cmd_clean(&cmd);
        } else {
            io_perror("Input parsing failed");
        }
    }
}
