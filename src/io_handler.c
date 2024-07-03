#include "io_handler.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

static IOConfig *_io_cfg = NULL;
static IOErrorCode _io_error_code = IO_SUCCESS;

IOErrorCode io_get_error_code(void) { return _io_error_code; }

const char *io_get_error_msg(IOErrorCode code) {
    switch (code) {
        case IO_SUCCESS:
            return "Success";
        case IO_CFG_NOT_INITIALIZED:
            return "IOConfig not initialized";
        case IO_CFG_ALREADY_INITIALIZED:
            return "IOConfig already initialized";
        case IO_INPUT_TOO_BIG:
            return "Input size was too big";
        case IO_SYSTEM_ERROR:
            return strerror(errno);
        default:  // should be unreachable and it's only here for completion
            return "Unknown error";
    }
}

void io_perror(const char *msg) {
    const char *err_msg = io_get_error_msg(_io_error_code);
    switch (_io_error_code) {
        case IO_SUCCESS:
        case IO_CFG_ALREADY_INITIALIZED:
        case IO_INPUT_TOO_BIG:
        case IO_SYSTEM_ERROR:
            io_write_err("%s: %s\n", msg, err_msg);
            break;
        case IO_CFG_NOT_INITIALIZED:
            fprintf(stderr, "%s: %s\n", msg, err_msg);
            break;
    }

    _io_error_code = IO_SUCCESS;
}

typedef struct IOConfig {
    FILE *out;
    FILE *in;
    FILE *err;
} IOConfig;

void io_alloc_cfg_singleton(FILE *out, FILE *in, FILE *err) {
    if (_io_cfg == NULL) {
        _io_cfg = malloc(sizeof(IOConfig));
        if (_io_cfg == NULL) {
            fprintf(stderr, "ERROR: IOConfig allocation failed\n");
            exit(EX_OSERR);
        }
        _io_cfg->out = out == NULL ? stdout : out;
        _io_cfg->in = in == NULL ? stdin : in;
        _io_cfg->err = err == NULL ? stderr : err;
    } else {
        _io_error_code = IO_CFG_ALREADY_INITIALIZED;
    }
}

void io_free_cfg_singleton(void) {
    free(_io_cfg);
    _io_cfg = NULL;
    _io_error_code = IO_SUCCESS;
}

const IOConfig *io_get_cfg_instance(void) {
    if (_io_cfg == NULL) {
        _io_error_code = IO_CFG_NOT_INITIALIZED;
        return NULL;
    }
    return _io_cfg;
}

int32_t io_read(char *buf, size_t size) {
    if (_io_cfg == NULL) {
        _io_error_code = IO_CFG_NOT_INITIALIZED;
        return -1;
    }

    char *read_result = fgets(buf, (int32_t)size, _io_cfg->in);
    if (read_result == NULL) {
        _io_error_code = IO_SYSTEM_ERROR;
        return -1;
    }

    bool input_was_too_big = strlen(buf) == size - 1 && buf[size - 2] != '\n';
    if (input_was_too_big) {
        _io_error_code = IO_INPUT_TOO_BIG;
        return -1;
    }

    _io_error_code = IO_SUCCESS;

    return (int32_t)strlen(buf);
}

int32_t io_write(const char *fmt, ...) {
    if (_io_cfg == NULL) {
        _io_error_code = IO_CFG_NOT_INITIALIZED;
        return -1;
    }

    va_list args;
    va_start(args, fmt);
    int32_t chars_written = vfprintf(_io_cfg->out, fmt, args);
    va_end(args);

    _io_error_code = IO_SUCCESS;

    return chars_written;
}

int32_t io_write_err(const char *fmt, ...) {
    if (_io_cfg == NULL) {
        _io_error_code = IO_CFG_NOT_INITIALIZED;
        return -1;
    }

    va_list args;
    va_start(args, fmt);
    int32_t chars_written = vfprintf(_io_cfg->err, fmt, args);
    va_end(args);

    _io_error_code = IO_SUCCESS;

    return chars_written;
}

int32_t io_flush_out_stream(void) {
    int32_t result = fflush(_io_cfg->out);
    _io_error_code = result == 0 ? IO_SUCCESS : IO_SYSTEM_ERROR;
    return result;
}

int32_t io_flush_err_stream(void) {
    int32_t result = fflush(_io_cfg->err);
    _io_error_code = result == 0 ? IO_SUCCESS : IO_SYSTEM_ERROR;
    return result;
}

inline bool io_input_was_too_big(void) { return _io_error_code == IO_INPUT_TOO_BIG; }

void io_clean_input_stream(void) {
    if (_io_cfg == NULL) {
        _io_error_code = IO_CFG_NOT_INITIALIZED;
        return;
    }

    int c;
    while ((c = getc(_io_cfg->in)) != '\n' && c != EOF);
}
