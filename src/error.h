#pragma once

typedef enum {
    SUCCESS,
    IO_CFG_ALREADY_INITIALIZED,
    IO_INPUT_TOO_BIG,
    REDIRECTION_WITHOUT_FILENAME,
    CMD_NOT_FOUND,
    HISTORY_OPEN_FAILED,
    HISTORY_READ_FAILED,
    HISTORY_WRITE_FAILED,
    SYSTEM_ERROR,
} ErrorCode;

ErrorCode get_error_code(void);
void set_error_code(ErrorCode code);
const char *get_error_msg(ErrorCode code);
const char *get_current_error_msg(void);
