#include "cmd_parser.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

void cmd_init(Command *cmd) {
    cmd->_args = NULL;
    cmd->_size = 0;
    cmd->_capacity = 8;
}

void cmd_free(Command *cmd) {
    free(cmd->_args);
    cmd->_args = NULL;
    cmd->_size = 0;
    cmd->_capacity = 0;
}

void cmd_clean(Command *cmd) { cmd->_size = 0; }

static int8_t _cmd_add_arg(Command *cmd, char *arg) {
    if (cmd->_args == NULL) {
        cmd->_args = malloc(cmd->_capacity * sizeof(char *));
        if (cmd->_args == NULL) {
            set_error_code(SYSTEM_ERROR);
            return -1;
        }
    } else if (cmd->_size == cmd->_capacity) {
        char **tmp = realloc(cmd->_args, cmd->_capacity * 2 * sizeof(char *));
        if (tmp == NULL) {
            set_error_code(SYSTEM_ERROR);
            return -1;
        }

        cmd->_args = tmp;
        cmd->_capacity *= 2;
    }

    cmd->_args[cmd->_size++] = arg;
    return 0;
}

static char *_get_next_token(char **str) {
    char *start = *str;
    while (*start && isspace(*start)) {
        start++;
    }

    if (*start == '\0') {
        return NULL;
    }

    char *token = start;
    bool in_quotes = false;
    while (*start) {
        if (*start == '"') {
            in_quotes = !in_quotes;
        } else if (!in_quotes && isspace(*start)) {
            break;
        }
        start++;
    }

    if (*start) {
        *start++ = '\0';
    }
    *str = start;

    if (*token == '"') {
        token++;
        char *end = token + strlen(token) - 1;
        if (*end == '"') {
            *end = '\0';
        }
    }

    return token;
}

int8_t cmd_parse_input(Command *cmd, char *input) {
    int8_t error_code = 0;
    char *tk;
    while ((tk = _get_next_token(&input)) != NULL) {
        error_code = _cmd_add_arg(cmd, tk);
        if (error_code != 0) {
            return error_code;
        }
    }
    error_code = _cmd_add_arg(cmd, NULL);

    return error_code;
}

const char *cmd_get_mnemonic(Command *cmd) { return cmd->_args[0]; }

char *const *cmd_get_args(Command *cmd) { return cmd->_args; }
