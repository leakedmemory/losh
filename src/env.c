#include "env.h"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sysexits.h>
#include <unistd.h>

#define PATH "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:."

typedef struct {
    const char *name;
    char *value;
} EnvVariable;

static EnvVariable *_vars_backup = NULL;
static size_t _vars_size = 0;

static void _backup_vars(EnvVariable *vars) {
    _vars_backup = malloc(_vars_size * sizeof(EnvVariable));
    if (_vars_backup == NULL) {
        perror("ERROR: Memory allocation for environment variables backup failed");
        exit(EX_OSERR);
    }

    for (size_t i = 0; i < _vars_size; i++) {
        _vars_backup[i].name = vars[i].name;
        const char *env_value = getenv(vars[i].name);
        _vars_backup[i].value = env_value ? strdup(env_value) : NULL;
        if (env_value && _vars_backup[i].value == NULL) {
            perror("ERROR: Memory allocation for environment variable value failed");
            exit(EX_OSERR);
        }
    }
}

static void _set_vars(const EnvVariable *vars) {
    for (size_t i = 0; i < _vars_size; i++) {
        const EnvVariable var = vars[i];
        // value can be NULL when restoring the backup
        if (var.value != NULL) {
            if (setenv(var.name, var.value, 1) == -1) {
                perror("ERROR: Setting environment variables failed");
                exit(EX_OSERR);
            }
        }
    }
}

void env_init(void) {
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (pw == NULL) {
        perror("ERROR: Getting user's passwd structure failed");
        exit(EX_OSERR);
    }

    if (chdir(pw->pw_dir) == -1) {
        perror("ERROR: Changing current working directory failed");
        exit(EX_OSERR);
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
    // remember to change the tests if any var is added
    EnvVariable vars[] = {{"USER", pw->pw_name},
                          {"HOME", pw->pw_dir},
                          {"PWD", pw->pw_dir},
                          {"PATH", (char *)PATH},
                          {"SHELL", (char *)"losh"}};
#pragma GCC diagnostic pop

    _vars_size = sizeof(vars) / sizeof(vars[0]);
    _backup_vars(vars);
    _set_vars(vars);
}

void env_restore(void) {
    _set_vars(_vars_backup);
    for (size_t i = 0; i < _vars_size; i++) {
        free(_vars_backup[i].value);
    }
    free(_vars_backup);
    _vars_backup = NULL;
    _vars_size = 0;
}
