#pragma once

typedef enum {
    SUCCESS,
    IO_CFG_ALREADY_INITIALIZED,
    IO_INPUT_TOO_BIG,
    CMD_NOT_FOUND,
    SYSTEM_ERROR,
} ErrorCode;

ErrorCode get_error_code(void);
void set_error_code(ErrorCode code);
const char *get_error_msg(ErrorCode code);
const char *get_current_error_msg(void);
