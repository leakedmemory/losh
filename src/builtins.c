// needed to use `setenv`, `chdir` and `strdup` functions
#define _POSIX_C_SOURCE 200809L

#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "find_cmd.h"
#include "lib.h"

// must be the amount of builtin commands
#define BUILTINS_AMOUNT 6

// builtin commands definitions - initializations at the end of file
static int32_t cd(char **args);
static int32_t builtin_exit(char **args);
static int32_t echo(char **args);
static int32_t pwd(char **args);
static int32_t where(char **args);
static int32_t which(char **args);

// macro for easier node creation
#define NODE(name, handler, min_args, max_args, next) \
    { name, {handler, min_args, max_args}, next }

typedef struct hash_node {
    const char *key;
    Builtin cmd;
    struct hash_node *next;
} HashNode;

static size_t hash(const char *str) {
    uint64_t hash = 5381;
    int32_t c;

    while ((c = *(unsigned char *)str++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return hash % BUILTINS_AMOUNT;
}

static HashNode *builtins_hashtable[BUILTINS_AMOUNT] = {NULL};

static void insert_node(HashNode *node) {
    size_t index = hash(node->key);
    HashNode *new_node = malloc(sizeof(HashNode));
    memcpy(new_node, node, sizeof(HashNode));
    if (!builtins_hashtable[index]) {
        builtins_hashtable[index] = new_node;
    } else {
        HashNode *current = builtins_hashtable[index];
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;
    }
}

void init_builtins(void) {
    HashNode nodes[BUILTINS_AMOUNT] = {
        NODE("cd", cd, 0, 1, NULL),        NODE("exit", builtin_exit, 0, 0, NULL),
        NODE("echo", echo, 0, -1, NULL),   NODE("pwd", pwd, 0, 0, NULL),
        NODE("where", where, 0, -1, NULL), NODE("which", which, 0, -1, NULL)};

    for (size_t i = 0; i < BUILTINS_AMOUNT; i++) {
        insert_node(&nodes[i]);
    }
}

Builtin const *get_builtin(const char *cmd) {
    size_t index = hash(cmd);
    if (builtins_hashtable[index]) {
        HashNode *current = builtins_hashtable[index];
        while (current) {
            if (strcmp(current->key, cmd) == 0) return &current->cmd;

            current = current->next;
        }
    }

    return NULL;
}

// TODO: make go back using ".."
// FIXME: breaking with `cd dev/cpp` and others
static int32_t cd(char **args) {
    int32_t status = 0;
    const char *path = args[1];
    if (path == NULL) {
        const char *home = getenv("HOME");
        if (chdir(home) == 0) {
            setenv("PWD", home, 1);
        }
    } else {
        const char *new_dir = join_paths(getenv("PWD"), path);
        if (chdir(new_dir) == 0) {
            setenv("PWD", new_dir, 1);
        }
    }

    return status;
}

static int32_t builtin_exit(char **args) {
    (void)args;
    exit(EXIT_SUCCESS);
}

static int32_t echo(char **args) {
    int32_t status = 0;
    StrVector *sv = alloc_str_vector();
    size_t i = 1;
    for (char *arg = args[i]; arg != NULL; arg = args[++i]) {
        if (arg[0] == '$') {
            char *tmp = getenv(arg + 1);
            if (tmp == NULL) {
                continue;
            } else {
                arg = tmp;
            }
        }
        add_str(sv, arg);
    }
    printf("%s\n", concatenate_strs(sv, ' '));
    free_str_vector(sv);

    return status;
}

static int32_t pwd(char **args) {
    (void)args;
    int32_t status = 0;
    char *pwd = getenv("PWD");
    if (pwd == NULL) {
        fprintf(stderr, "ERROR: can't find $PWD enviroment variable");
        status = -1;
    } else {
        puts(pwd);
    }

    return status;
}

static bool is_builtin(const char *cmd) { return get_builtin(cmd) != NULL; }

static int32_t where(char **args) {
    int32_t status = 0;
    const char *arg = args[1];
    bool is_bltin = is_builtin(arg);
    if (is_bltin) {
        printf("%s: shell built-in command\n", arg);
    }

    char *paths_found = find_all_command(arg);
    if (paths_found != NULL) {
        puts(paths_found);
        free(paths_found);
    } else if (!is_bltin) {
        status = -1;
    }

    return status;
}

static int32_t which(char **args) {
    int32_t status = 0;
    const char *arg = args[1];
    if (is_builtin(arg)) {
        printf("%s: shell built-in command\n", arg);
    } else {
        char *result = find_command(arg);
        if (result != NULL) {
            puts(result);
            free(result);
        } else {
            status = -1;
        }
    }

    return status;
}
