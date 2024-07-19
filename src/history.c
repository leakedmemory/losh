#include "history.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "io_handler.h"
#include "utils.h"

// PERF: probably shouldn't open and close the file for every IO

int8_t save_cmd_in_history(const char *cmd, size_t size, const char *filepath) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
    char *path;
    if (filepath[0] == '~') {
        path = expand_tilde(filepath);
        if (path == NULL) {
            return -1;
        }
    } else {
        path = (char *)filepath;
    }
#pragma GCC diagnostic pop

    FILE *history = fopen(path, "a");
    if (history == NULL) {
        if (filepath[0] == '~') {
            free(path);
        }
        set_error_code(HISTORY_OPEN_FAILED);
        return -1;
    }

    if (fwrite(cmd, sizeof(char), size, history) != size) {
        if (filepath[0] == '~') {
            free(path);
        }
        set_error_code(HISTORY_WRITE_FAILED);
        fclose(history);
        return -1;
    }

    if (filepath[0] == '~') {
        free(path);
    }
    fclose(history);

    return 0;
}

int8_t print_cmd_history(const char *filepath) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
    char *path;
    if (filepath[0] == '~') {
        path = expand_tilde(filepath);
        if (path == NULL) {
            return -1;
        }
    } else {
        path = (char *)filepath;
    }
#pragma GCC diagnostic pop

    FILE *history = fopen(path, "r");
    if (history == NULL) {
        if (filepath[0] == '~') {
            free(path);
        }
        set_error_code(HISTORY_OPEN_FAILED);
        return -1;
    }

    int32_t buf_size = 4096;
    char buf[buf_size];
    for (size_t n_line = 1; fgets(buf, buf_size, history) != NULL; n_line++) {
        io_write(" %lu  %s", n_line, buf);
    }

    if (!feof(history)) {
        if (filepath[0] == '~') {
            free(path);
        }
        set_error_code(HISTORY_READ_FAILED);
        fclose(history);
        return -1;
    }

    if (filepath[0] == '~') {
        free(path);
    }
    fclose(history);

    return 0;
}
