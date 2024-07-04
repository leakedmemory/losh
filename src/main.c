#include "io_handler.h"

#define INPUT_SIZE 4096

int main(void) {
    char input[INPUT_SIZE];
    io_alloc_cfg_singleton(stdout, stdin, stderr);

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

        // TODO: parse input
    }
}
