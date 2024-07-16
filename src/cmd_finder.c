#include "cmd_finder.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "error.h"
#include "io_handler.h"
#include "utils.h"

typedef enum { Binary, NotBinary } IsBinaryResult;

static IsBinaryResult _is_binary(const char *path) {
    struct stat file_info;
    return stat(path, &file_info) == 0 && file_info.st_mode & S_IXUSR ? Binary : NotBinary;
}

/* caller should free the return after use */
char *cmd_find(const char *mnemonic) {
    char *env_path = strdup(getenv("PATH"));
    if (env_path == NULL) {
        set_error_code(SYSTEM_ERROR);
        io_perror("Getting PATH environment variable failed");
        return NULL;
    }

    const char *delim = ":";
    char *exec_path = strtok(env_path, delim);
    while (exec_path) {
        char *full_path = path_concat(exec_path, mnemonic);
        if (full_path == NULL) {
            io_perror("Paths concatenation failed");
            return NULL;
        }

        switch (_is_binary(full_path)) {
            case Binary:
                free(env_path);
                return full_path;
            case NotBinary:
                free(full_path);
                break;
        }

        exec_path = strtok(NULL, delim);
    }

    set_error_code(CMD_NOT_FOUND);
    free(env_path);
    return NULL;
}

typedef struct {
    char **data;
    size_t size;
    size_t capacity;
} CmdMatches;

static void _cmd_matches_init(CmdMatches *matches) {
    matches->data = NULL;
    matches->size = 0;
    matches->capacity = 4;
}

static int8_t _cmd_matches_push(CmdMatches *matches, char *cmd) {
    if (matches->data == NULL) {
        matches->data = malloc(matches->capacity * sizeof(char *));
        if (matches->data == NULL) {
            set_error_code(SYSTEM_ERROR);
            return -1;
        }
    } else if (matches->size == matches->capacity) {
        char **tmp = realloc(matches->data, matches->capacity * 2 * sizeof(char *));
        if (tmp == NULL) {
            set_error_code(SYSTEM_ERROR);
            return -1;
        }

        matches->data = tmp;
        matches->capacity *= 2;
    }

    matches->data[matches->size++] = cmd;
    return 0;
}

/* caller should free the return and its elements after use */
char **cmd_find_all(const char *mnemonic) {
    char *env_path = strdup(getenv("PATH"));
    if (env_path == NULL) {
        set_error_code(SYSTEM_ERROR);
        io_perror("Getting PATH environment variable failed");
        return NULL;
    }

    CmdMatches matches;
    _cmd_matches_init(&matches);
    const char *delim = ":";
    char *exec_path = strtok(env_path, delim);
    while (exec_path) {
        char *full_path = path_concat(exec_path, mnemonic);
        if (full_path == NULL) {
            io_perror("Paths concatenation failed");
            return NULL;
        }

        switch (_is_binary(full_path)) {
            case Binary:
                if (_cmd_matches_push(&matches, full_path) != 0) {
                    io_perror("Pushing command match failed");
                }
                break;
            case NotBinary:
                free(full_path);
                break;
        }

        exec_path = strtok(NULL, delim);
    }

    if (matches.size > 0) {
        _cmd_matches_push(&matches, NULL);
    } else {
        set_error_code(CMD_NOT_FOUND);
    }

    free(env_path);

    return matches.data;
}
