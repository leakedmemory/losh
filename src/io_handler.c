#include "io_handler.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "error.h"

static IOConfig *_io_cfg = NULL;

void io_alloc_cfg_singleton(FILE *in, FILE *out, FILE *err) {
    if (_io_cfg == NULL) {
        _io_cfg = malloc(sizeof(IOConfig));
        if (_io_cfg == NULL) {
            // probably not the best way of handling it, since it's a core point
            // of the program and probably it would be better to take some tries
            // before exiting, but w/e
            perror("ERROR: IOConfig allocation failed");
            exit(EX_OSERR);
        }
        _io_cfg->in = in == NULL ? stdin : in;
        _io_cfg->out = out == NULL ? stdout : out;
        _io_cfg->err = err == NULL ? stderr : err;
        set_error_code(SUCCESS);
    } else {
        set_error_code(IO_CFG_ALREADY_INITIALIZED);
    }
}

void io_free_cfg_singleton(void) {
    free(_io_cfg);
    _io_cfg = NULL;
}

const IOConfig *io_instance(void) { return _io_cfg; }

static void _clean_input_stream(void) {
    int c;
    while ((c = getc(_io_cfg->in)) != '\n' && c != EOF);
}

int32_t io_read(char *buf, size_t size) {
    char *read_result = fgets(buf, (int32_t)size, _io_cfg->in);
    if (read_result == NULL) {
        if (ferror(_io_cfg->in)) {
            clearerr(_io_cfg->in);
        }
        return -1;
    }

    bool input_was_too_big = strlen(buf) == size - 1 && buf[size - 2] != '\n';
    if (input_was_too_big) {
        _clean_input_stream();
        set_error_code(IO_INPUT_TOO_BIG);
        return -1;
    }

    return (int32_t)strlen(buf);
}

int32_t io_puts(const char *s) { return fprintf(_io_cfg->out, "%s\n", s); }

int32_t io_write(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int32_t chars_written = vfprintf(_io_cfg->out, fmt, args);
    va_end(args);

    return chars_written;
}

int32_t io_write_err(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int32_t chars_written = vfprintf(_io_cfg->err, fmt, args);
    va_end(args);

    return chars_written;
}

int32_t io_flush(FILE *stream) {
    int32_t result = fflush(stream);
    set_error_code(result == 0 ? SUCCESS : SYSTEM_ERROR);
    return result;
}

void io_perror(const char *msg) {
    const char *err_msg = get_current_error_msg();
    switch (get_error_code()) {
        case SUCCESS:
            io_write_err("%s: %s\n", msg, err_msg);
            break;
        case IO_INPUT_TOO_BIG:
        case SYSTEM_ERROR:
            io_write_err("ERROR: %s: %s\n", msg, err_msg);
            break;
        case IO_CFG_ALREADY_INITIALIZED:
            io_write_err("WARNING: %s: %s\n", msg, err_msg);
            break;
        case CMD_NOT_FOUND:
            io_write_err("losh: %s: %s\n", err_msg, msg);
            break;
    }

    set_error_code(SUCCESS);
}
