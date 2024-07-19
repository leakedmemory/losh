#include "builtins.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "cmd_finder.h"
#include "error.h"
#include "history.h"
#include "io_handler.h"
#include "jobs.h"
#include "utils.h"

// should be a prime number
#define BUILTINS_HASH_MAP_CAPACITY 17

#define NODE(mnemonic, func, min_args, max_args) {mnemonic, {func, min_args, max_args}, NULL}

typedef struct HashNode {
    const char *key;
    BuiltinCmd builtin;
    struct HashNode *next;
} HashNode;

static HashNode *_hash_map[BUILTINS_HASH_MAP_CAPACITY] = {NULL};

static size_t _hash(const char *key) {
    uint32_t hash = 5381;
    unsigned char c;

    while ((c = *(const unsigned char *)key++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return hash % BUILTINS_HASH_MAP_CAPACITY;
}

static void _insert_builtin(HashNode *builtin) {
    size_t index = _hash(builtin->key);
    HashNode *node = malloc(sizeof(HashNode));
    if (node == NULL) {
        perror("ERROR: Built-ins allocation failed");
        exit(EX_OSERR);
    }

    memcpy(node, builtin, sizeof(HashNode));
    node->next = _hash_map[index];
    _hash_map[index] = node;
}

BuiltinCmd *get_builtin(const char *mnemonic) {
    HashNode *node = _hash_map[_hash(mnemonic)];
    while (node != NULL) {
        if (strcmp(node->key, mnemonic) == 0) {
            return &node->builtin;
        }
        node = node->next;
    }

    return NULL;
}

static bool _is_builtin(const char *mnemonic) { return get_builtin(mnemonic) != NULL; }

static int32_t _cd(char **args) {
    char *home = getenv("HOME");
    if (home == NULL) {
        io_write_err("ERROR: Could not find $HOME environment variable\n");
        return -1;
    }

    const char *pwd = getenv("PWD");
    if (pwd == NULL) {
        io_write_err("ERROR: Could not find $PWD environment variable\n");
        return -1;
    }

    char *path;
    if (args[1] != NULL && args[1][0] == '~') {
        path = expand_tilde(args[1]);
        if (path == NULL) {
            io_perror("Tilde expansion failed");
            return -1;
        }
    } else {
        path = args[1];
    }

    char *new_dir;
    bool should_free_new_dir = false;
    if (path == NULL) {
        new_dir = home;
    } else if (path[0] == '/' || strcmp(path, "..") == 0) {
        new_dir = path;
    } else {
        new_dir = path_concat(pwd, path);
        if (new_dir == NULL) {
            io_perror("Paths concatenation failed");
            return -1;
        }
        should_free_new_dir = true;
    }

    if (chdir(new_dir) == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            set_error_code(SYSTEM_ERROR);
            io_perror(
                "Getting current working directory while trying to change $PWD environment "
                "variable failed");
            if (should_free_new_dir) {
                free(new_dir);
            }
            return -1;
        }

        if (setenv("PWD", cwd, 1) == -1) {
            set_error_code(SYSTEM_ERROR);
            io_perror(
                "Setting $PWD environment variable failed. Current directory is now different "
                "from $PWD environment variable\n");
            if (should_free_new_dir) {
                free(new_dir);
            }
            return -1;
        }

        if (should_free_new_dir) {
            free(new_dir);
        }
        return 0;
    }

    set_error_code(SYSTEM_ERROR);
    io_perror("Change directory failed");

    if (should_free_new_dir) {
        free(new_dir);
    }
    return -1;
}

static int32_t _pwd(char **args) {
    (void)args;
    int32_t status = 0;
    const char *pwd = getenv("PWD");
    if (pwd != NULL) {
        io_puts(pwd);
    } else {
        io_write_err("ERROR: Could not find $PWD environment variable\n");
        status = -1;
    }

    return status;
}

static int32_t _losh_exit(char **args) {
    (void)args;
    exit(EXIT_SUCCESS);  // does not return
}

static int32_t _echo(char **args) {
    for (size_t i = 1; args[i] != NULL; i++) {
        if (i > 1) io_write(" ");

        if (args[i][0] == '$') {
            char *env_var_value = getenv(args[i] + 1);
            if (env_var_value != NULL) {
                io_write("%s", env_var_value);
            } else {
                io_write("%s", args[i]);
            }
        } else {
            io_write("%s", args[i]);
        }
    }
    io_write("\n");

    return 0;
}

static int32_t _which(char **args) {
    int32_t status = 0;
    char *bin = args[1];
    if (_is_builtin(bin)) {
        io_write("%s: shell built-in command\n", bin);
    } else {
        char *cmd_path = cmd_find(bin);
        if (cmd_path != NULL) {
            io_puts(cmd_path);
            free(cmd_path);
            cmd_path = NULL;
        } else {
            io_perror(bin);
            status = -1;
        }
    }

    return status;
}

static int32_t _where(char **args) {
    int32_t status = 0;
    char *bin = args[1];
    if (_is_builtin(bin)) {
        io_write("%s: shell built-in command\n", bin);
    }

    char **cmd_path = cmd_find_all(bin);
    if (cmd_path != NULL) {
        for (size_t i = 0; cmd_path[i] != NULL; i++) {
            io_puts(cmd_path[i]);
            free(cmd_path[i]);
        }
        free(cmd_path);
        cmd_path = NULL;
    } else if (!_is_builtin(bin)) {
        io_perror(bin);
        status = -1;
    }

    return status;
}

static int32_t _fg(char **args) {
    if (args[1] == NULL) {
        if (job_list_is_empty()) {
            io_write("fg: no current job\n");
        } else {
            print_jobs();
        }

        return 0;
    }

    int32_t job_id = atoi(args[1]);
    if (job_id == 0) {
        io_write_err("ERROR: Argument passed is a not a number: %s\n", args[1]);
    }

    return bring_to_foreground(job_id);
}

static int32_t _history(char **args) {
    (void)args;
    return print_cmd_history(CMD_HISTORY_PATH);
}

void builtins_init(void) {
    // make sure the size is less than or equal to BUILTINS_HASH_MAP_CAPACITY
    // when adding a new builtin command
    HashNode builtins[] = {NODE("pwd", _pwd, 0, 0),        NODE("cd", _cd, 0, 1),
                           NODE("exit", _losh_exit, 0, 0), NODE("echo", _echo, 0, UINT32_MAX),
                           NODE("which", _which, 1, 1),    NODE("where", _where, 1, 1),
                           NODE("fg", _fg, 0, 1),          NODE("history", _history, 0, 0)};

    size_t builtins_size = sizeof(builtins) / sizeof(builtins[0]);
    for (size_t i = 0; i < builtins_size; i++) {
        _insert_builtin(&builtins[i]);
    }
}

void builtins_free(void) {
    for (size_t i = 0; i < BUILTINS_HASH_MAP_CAPACITY; ++i) {
        HashNode *curr = _hash_map[i];
        while (curr != NULL) {
            HashNode *next = curr->next;
            free(curr);
            curr = next;
        }
        _hash_map[i] = NULL;
    }
}
