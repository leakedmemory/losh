#include "cmd_exec.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "cmd_finder.h"
#include "cmd_parser.h"
#include "error.h"
#include "io_handler.h"
#include "jobs.h"

static int32_t _redirect_io(Command *cmd, ExecFunction func) {
    int32_t original_stdin = dup(STDIN_FILENO);
    if (original_stdin == -1) {
        return -1;
    }

    int32_t original_stdout = dup(STDOUT_FILENO);
    if (original_stdout == -1) {
        close(original_stdin);
        return -1;
    }

    if (cmd->input_file != NULL) {
        int32_t in_fd = open(cmd->input_file, O_RDONLY);
        if (in_fd == -1) {
            close(original_stdin);
            close(original_stdout);
            return -1;
        }

        if (dup2(in_fd, STDIN_FILENO) == -1) {
            close(in_fd);
            close(original_stdin);
            close(original_stdout);
            return -1;
        }
        close(in_fd);
    }

    if (cmd->output_file != NULL) {
        int32_t out_fd =
            open(cmd->output_file, O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC), 0644);
        if (out_fd == -1) {
            close(original_stdin);
            close(original_stdout);
            return -1;
        }

        if (dup2(out_fd, STDOUT_FILENO) == -1) {
            close(out_fd);
            close(original_stdin);
            close(original_stdout);
            return -1;
        }
        close(out_fd);
    }

    int32_t result = func(cmd->args);
    io_flush(stdout);

    if (dup2(original_stdin, STDIN_FILENO) == -1) {
        close(original_stdin);
        close(original_stdout);
        return -1;
    }
    close(original_stdin);

    if (dup2(original_stdout, STDOUT_FILENO) == -1) {
        close(original_stdout);
        return -1;
    }
    close(original_stdout);

    return result;
}

static int32_t _fork_exec(Command *cmd, ExecFunction func) {
    pid_t pid = fork();
    if (pid == -1) {
        set_error_code(SYSTEM_ERROR);
        return -1;
    } else if (pid == 0) {
        int32_t status_code = 0;
        if (cmd->output_file != NULL || cmd->input_file != NULL) {
            status_code = _redirect_io(cmd, func);
        } else {
            status_code = func(cmd->args);
        }
        exit(status_code == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
    } else {
        if (cmd->is_background) {
            int32_t job_id = add_job(pid, cmd->args[0]);
            if (job_id == -1) {
                io_write_err("ERROR: Job list is full\n");
                return -1;
            }

            io_write("[%d] %d\n", job_id, pid);
            return 0;
        } else {
            int32_t pid_status;
            waitpid(pid, &pid_status, 0);
            if (!WIFEXITED(pid_status) || WEXITSTATUS(pid_status) != EXIT_SUCCESS) {
                set_error_code(SYSTEM_ERROR);
                return -1;
            }
            return 0;
        }
    }
}

static int32_t _exec_bin(char **args) {
    const char *mnemonic = args[0];
    char *cmd_path = cmd_find(mnemonic);
    if (cmd_path == NULL) {
        io_perror(mnemonic);
        return -1;
    }

    execv(cmd_path, args);
    free(cmd_path);
    return -1;
}

void cmd_exec(Command *cmd) {
    const char *mnemonic = cmd->args[0];
    BuiltinCmd *builtin = get_builtin(mnemonic);
    if (builtin == NULL) {
        _fork_exec(cmd, _exec_bin);
    } else {
        // $ min_args <= number_of_args <= max_args $
        bool has_valid_size_of_args =
            builtin->min_args <= cmd->size - 2 && cmd->size - 2 <= builtin->max_args;

        if (has_valid_size_of_args) {
            if (cmd->is_background) {
                _fork_exec(cmd, builtin->func);
            } else if (cmd->output_file != NULL || cmd->input_file != NULL) {
                _redirect_io(cmd, builtin->func);
            } else {
                builtin->func(cmd->args);
            }
        } else {
            io_write_err(
                "ERROR: Invalid numbers of arguments: Expects min of %lu and max of %lu args\n",
                builtin->min_args, builtin->max_args);
        }
    }
}
