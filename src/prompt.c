#include "prompt.h"

#include <stdbool.h>
#include <stdio.h>
#include <termio.h>

#include "io_handler.h"
#include "signal_handler.h"

void write_prompt(void) {
    if (input_buffer_is_dirty) {
        // discards data written to the object referred to by fd but not transmitted
        tcflush(fileno(io_instance()->in), TCIFLUSH);
        input_buffer_is_dirty = false;
    }
    io_write("losh$ ");
    io_flush(io_instance()->out);
}
