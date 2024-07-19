#include "prompt.h"

#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <unistd.h>

#include "io_handler.h"
#include "signal_handler.h"

void write_prompt(void) {
    if (input_buffer_is_dirty) {
        // discards data written to the object referred to by fd but not transmitted
        tcflush(fileno(io_instance()->in), TCIFLUSH);
        input_buffer_is_dirty = false;
    }

    const char *user = getenv("USER");
    const char *cwd = basename(getenv("PWD"));
    const char symbol = getuid() == 0 ? '#' : '$';
    if (strcmp(cwd, getenv("USER")) == 0) {
        io_write("%s | ~ %c> ", user, symbol);
    } else {
        io_write("%s | %s %c> ", user, cwd, symbol);
    }
    io_flush(io_instance()->out);
}
