// needed to use `strdup` function
#define _POSIX_C_SOURCE 200809L

#include "find_cmd.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "lib.h"

char *join_paths(const char *root, const char *relative) {
    size_t length = strlen(root) + 1 + strlen(relative) + 1;
    char *result = calloc(length, sizeof(char));
    if (result == NULL) {
        return NULL;
    }
    strcat(result, root);
    strcat(result, "/");
    strcat(result, relative);
    strcat(result, "\0");
    return result;
}

static bool is_binary(const char *path) {
    struct stat file_info;
    if (stat(path, &file_info) == 0) {
        return file_info.st_mode & S_IXUSR;
    }

    return false;
}

char *find_all_command(const char *cmd) {
    StrVector *sv = alloc_str_vector();
    const char *delim = ":";
    char *env_path = strdup(getenv("PATH"));
    char *dir_path = strtok(env_path, delim);
    while (dir_path) {
        char *cmd_path = join_paths(dir_path, cmd);
        if (cmd_path == NULL) {
            free(env_path);
            return NULL;
        }

        if (is_binary(cmd_path)) {
            int32_t successfull = add_str(sv, cmd_path);
            if (successfull == -1) {
                free(env_path);
                return NULL;
            }
        }

        free(cmd_path);
        dir_path = strtok(NULL, delim);
    }

    if (sv->strs == NULL) {
        free(env_path);
        return NULL;
    }

    char *result = concatenate_strs(sv, '\n');
    free_str_vector(sv);
    free(env_path);
    return result;
}

char *find_command(const char *cmd) {
    const char *delim = ":";
    char *env_path = strdup(getenv("PATH"));
    char *dir_path = strtok(env_path, delim);
    while (dir_path) {
        char *cmd_path = join_paths(dir_path, cmd);
        if (is_binary(cmd_path)) {
            free(env_path);
            return cmd_path;
        }
        free(cmd_path);
        dir_path = strtok(NULL, delim);
    }

    free(env_path);
    return NULL;
}
