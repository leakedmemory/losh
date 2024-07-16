#include "cmd_exec.h"

#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "cmd_finder.h"
#include "cmd_parser.h"
#include "error.h"
#include "io_handler.h"

void cmd_exec(Command *cmd) {
    const char *mnemonic = cmd->args[0];
    BuiltinCmd *builtin = get_builtin(mnemonic);
    if (builtin != NULL) {
        // $ min_args <= number_of_args <= max_args $
        bool has_valid_size_of_args =
            builtin->min_args <= cmd->size - 2 && cmd->size - 2 <= builtin->max_args;

        if (has_valid_size_of_args) {
            builtin->func(cmd->args);
        } else {
            io_write_err(
                "ERROR: Invalid numbers of arguments: Expects min of %lu and max of %lu args\n",
                builtin->min_args, builtin->max_args);
        }
        io_write("\n");

        return;
    }

    char *cmd_path = cmd_find(mnemonic);
    if (cmd_path == NULL) {
        io_perror(mnemonic);
        io_write("\n");
        return;
    }

    pid_t pid = fork();
    int32_t pid_status;
    if (pid == -1) {
        set_error_code(SYSTEM_ERROR);
        io_perror("Forking new process failed");
    } else if (pid == 0) {
        // `execv` only returns on error
        execv(cmd_path, cmd->args);
        exit(EXIT_FAILURE);
    } else {
        waitpid(pid, &pid_status, 0);
        if (pid_status != EXIT_SUCCESS) {
            set_error_code(SYSTEM_ERROR);
            io_perror("Command execution failed");
        }
        printf("\n");
        free(cmd_path);
    }
}
