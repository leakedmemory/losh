#include "exec_cmd.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "cmd_parser.h"
#include "find_cmd.h"

typedef int32_t (*ExecFunction)(char **args);

// TODO: find better way to pass these arguments
static int32_t fork_exec(ExecFunction func, char **args) {
    pid_t pid = fork();
    int32_t pid_status = 0;
    if (pid == -1) {
        // TODO: handle error
    } else if (pid == 0) {
        int32_t status = func(args);
        if (status == 0) {
            exit(EXIT_SUCCESS);
        } else {
            exit(EXIT_FAILURE);
        }
    } else {
        waitpid(pid, &pid_status, 0);
        printf("\n");
    }

    return pid_status;
}

static inline int32_t exec_binary(char **args) {
    const char *cmd_path = find_command(args[0]);
    return execv(cmd_path, args);
}

int32_t exec_cmd(Command *cmd) {
    int32_t status = 0;

    if (cmd == NULL) {
        status = -1;
        return status;
    }

    char **args = cmd->args;
    Builtin const *builtin = get_builtin(args[0]);
    // TODO: handle errors of incorrect number of min_args and max_args sizes
    // in builtin execution
    if (builtin == NULL) {
        status = fork_exec(exec_binary, args);
    } else {
        status = builtin->handler(args);
        printf("\n");
    }

    return status;
}
