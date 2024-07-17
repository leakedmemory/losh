#pragma once

#include <stdint.h>
#include <stdio.h>

typedef struct {
    FILE *in;
    FILE *out;
    FILE *err;
} IOConfig;

void io_alloc_cfg_singleton(FILE *in, FILE *out, FILE *err);
void io_free_cfg_singleton(void);
const IOConfig *io_instance(void);

int32_t io_read(char *buf, size_t size);
int32_t io_puts(const char *s);
int32_t io_write(const char *fmt, ...);
int32_t io_write_err(const char *fmt, ...);
int32_t io_flush(FILE *stream);
void io_perror(const char *msg);
