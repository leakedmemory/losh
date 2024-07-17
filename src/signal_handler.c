#include "signal_handler.h"

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "io_handler.h"
#include "jobs.h"
#include "prompt.h"

volatile sig_atomic_t input_buffer_is_dirty = false;

// background process
static void _sigchld_handler(int32_t sig) {
    (void)sig;
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        Job *job = get_job(pid);
        if (job != NULL) {
            io_write("\n[%d] %d done: %s\n\n", job->id, job->pid, job->mnemonic);
            input_buffer_is_dirty = true;
            free(job->mnemonic);
            job->pid = 0;
            job->id = 0;
            write_prompt();
            break;
        }
    }
}

// ctrl + c
static void _sigint_handler(int32_t sig) {
    (void)sig;
    io_write("\n\n");
    input_buffer_is_dirty = true;
    write_prompt();
}

// ctrl + z
static void _sigtstp_handler(int32_t sig) {
    (void)sig;
    io_write_err("\nWARNING: Received SIGTSTP, but not suspending\n\n");
    io_flush(io_instance()->err);
    input_buffer_is_dirty = true;
    write_prompt();
}

void set_signal_handlers(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = _sigchld_handler;
    sigemptyset(&sa.sa_mask);
    // restart syscalls if interrupted by handler, don't stop on child stopped/continued
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("ERROR: Could not set SIGCHLD handler");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = _sigint_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("ERROR: Could not set SIGINT handler");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = _sigtstp_handler;
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        perror("ERROR: Could not set SIGTSTP handler");
        exit(EXIT_FAILURE);
    }
}
