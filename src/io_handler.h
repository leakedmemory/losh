#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
    IO_SUCCESS,
    IO_CFG_NOT_INITIALIZED,
    IO_CFG_ALREADY_INITIALIZED,
    IO_INPUT_TOO_BIG,
    IO_SYSTEM_ERROR,
} IOErrorCode;

IOErrorCode io_get_error_code(void);
const char *io_get_error_msg(IOErrorCode code);
void io_perror(const char *msg);

typedef struct IOConfig IOConfig;

void io_alloc_cfg_singleton(FILE *out, FILE *in, FILE *err);
void io_free_cfg_singleton(void);
const IOConfig *io_get_cfg_instance(void);

int32_t io_read(char *buf, size_t size);
int32_t io_write(const char *fmt, ...);
int32_t io_write_err(const char *fmt, ...);
int32_t io_flush_out_stream(void);
int32_t io_flush_err_stream(void);

bool io_input_was_too_big(void);
void io_clean_input_stream(void);
