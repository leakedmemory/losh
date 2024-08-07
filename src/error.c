#include "error.h"

#include <errno.h>
#include <string.h>

static ErrorCode _error_code = SUCCESS;

ErrorCode get_error_code(void) { return _error_code; }

void set_error_code(ErrorCode code) { _error_code = code; }

const char *get_error_msg(ErrorCode code) {
    switch (code) {
        case SUCCESS:
            return "Success";
        case IO_CFG_ALREADY_INITIALIZED:
            return "IOConfig already initialized";
        case IO_INPUT_TOO_BIG:
            return "Input size was too big";
        case REDIRECTION_WITHOUT_FILENAME:
            return "Path to file redirection not specified";
        case CMD_NOT_FOUND:
            return "Command not found";
        case HISTORY_READ_FAILED:
            return "Could not read command history";
        case HISTORY_WRITE_FAILED:
            return "Could not save command in history";
        case HISTORY_OPEN_FAILED:
        case SYSTEM_ERROR:
            return strerror(errno);
    }

    // should be unreachable and it's only here for completion
    return "Unknown error";
}

const char *get_current_error_msg(void) { return get_error_msg(_error_code); }
