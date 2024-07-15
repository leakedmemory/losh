#include "utils.h"

#include <stdlib.h>
#include <string.h>

#include "error.h"

char *path_concat(const char *parent_path, const char *relative_path) {
    size_t length = strlen(parent_path) + strlen(relative_path) + 2 * sizeof(char);
    char *result = calloc(length, sizeof(char));
    if (result == NULL) {
        set_error_code(SYSTEM_ERROR);
        return NULL;
    }

    strcat(result, parent_path);
    strcat(result, "/");
    strcat(result, relative_path);
    strcat(result, "\0");
    return result;
}
