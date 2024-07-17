#include "cmd_parser.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

#define TK_REDIRECT_INPUT "<"
#define TK_REDIRECT_OUTPUT_WRITE ">"
#define TK_REDIRECT_OUTPUT_APPEND ">>"

void cmd_init(Command *cmd) {
    cmd->args = NULL;
    cmd->size = 0;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append = false;
    cmd->_capacity = 8;
}

void cmd_free(Command *cmd) {
    free(cmd->args);
    cmd->args = NULL;
    cmd->size = 0;
    free(cmd->input_file);
    cmd->input_file = NULL;
    free(cmd->output_file);
    cmd->output_file = NULL;
    cmd->append = false;
    cmd->_capacity = 0;
}

void cmd_clean(Command *cmd) {
    cmd->size = 0;
    free(cmd->input_file);
    cmd->input_file = NULL;
    free(cmd->output_file);
    cmd->output_file = NULL;
    cmd->append = false;
}

static int8_t _cmd_add_arg(Command *cmd, char *arg) {
    if (cmd->args == NULL) {
        cmd->args = malloc(cmd->_capacity * sizeof(char *));
        if (cmd->args == NULL) {
            set_error_code(SYSTEM_ERROR);
            return -1;
        }
    } else if (cmd->size == cmd->_capacity) {
        char **tmp = realloc(cmd->args, cmd->_capacity * 2 * sizeof(char *));
        if (tmp == NULL) {
            set_error_code(SYSTEM_ERROR);
            return -1;
        }

        cmd->args = tmp;
        cmd->_capacity *= 2;
    }

    cmd->args[cmd->size++] = arg;
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
        if (strcmp(tk, TK_REDIRECT_OUTPUT_WRITE) == 0 ||
            strcmp(tk, TK_REDIRECT_OUTPUT_APPEND) == 0) {
            cmd->append = strcmp(tk, TK_REDIRECT_OUTPUT_APPEND) == 0;
            tk = _get_next_token(&input);
            if (tk != NULL) {
                cmd->output_file = strdup(tk);
                if (cmd->output_file == NULL) {
                    set_error_code(SYSTEM_ERROR);
                    return -1;
                }
            } else {
                set_error_code(REDIRECTION_WITHOUT_FILENAME);
                return -1;
            }
        } else if (strcmp(tk, TK_REDIRECT_INPUT) == 0) {
            tk = _get_next_token(&input);
            if (tk != NULL) {
                cmd->input_file = strdup(tk);
                if (cmd->input_file == NULL) {
                    set_error_code(SYSTEM_ERROR);
                    return -1;
                }
            } else {
                set_error_code(REDIRECTION_WITHOUT_FILENAME);
                return -1;
            }
        } else {
            error_code = _cmd_add_arg(cmd, tk);
            if (error_code != 0) {
                return error_code;
            }
        }
    }
    error_code = _cmd_add_arg(cmd, NULL);

    return error_code;
}
