#pragma once

#include <stdint.h>
#include <stdio.h>

typedef struct IOConfig IOConfig;

void io_alloc_cfg_singleton(FILE *out, FILE *in, FILE *err);
void io_free_cfg_singleton(void);
const IOConfig *io_get_cfg_instance(void);
const FILE *io_get_out_stream(void);
const FILE *io_get_in_stream(void);
const FILE *io_get_err_stream(void);

int32_t io_read(char *buf, size_t size);
int32_t io_write(const char *fmt, ...);
int32_t io_write_err(const char *fmt, ...);
int32_t io_flush_out_stream(void);
int32_t io_flush_err_stream(void);
void io_perror(const char *msg);
