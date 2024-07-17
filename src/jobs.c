#include "jobs.h"

#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "io_handler.h"

#define MAX_JOBS 100

static Job _jobs[MAX_JOBS] = {0};
static int32_t _next_job_id = 0;
static int32_t _jobs_left = MAX_JOBS;

int32_t add_job(pid_t pid, char *mnemonic) {
    for (int32_t i = 0; i < MAX_JOBS; i++) {
        int32_t job_idx = (_next_job_id + i) % MAX_JOBS;
        if (_jobs[job_idx].id == 0) {
            _jobs[job_idx].id = job_idx % MAX_JOBS + 1;
            _jobs[job_idx].pid = pid;
            _jobs[job_idx].mnemonic = strdup(mnemonic);
            _next_job_id = (job_idx + 1) % MAX_JOBS;
            _jobs_left--;

            return _jobs[job_idx].id;
        }
    }

    return -1;
}

int32_t bring_to_foreground(int32_t id) {
    for (size_t i = 0; i < MAX_JOBS; i++) {
        if (_jobs[i].id == id) {
            kill(_jobs[i].pid, SIGCONT);
            waitpid(_jobs[i].pid, NULL, 0);
            free(_jobs[i].mnemonic);
            _jobs[i].mnemonic = NULL;
            _jobs[i].pid = 0;
            _jobs[i].id = 0;
            _jobs_left++;
            return 0;
        }
    }

    io_write_err("ERROR: Could not find job with id %d\n", id);
    return -1;
}

void print_jobs(void) {
    for (size_t i = 0; i < MAX_JOBS; i++) {
        if (_jobs[i].pid != 0) {
            io_write("[%d] %d running: %s\n", _jobs[i].id, _jobs[i].pid, _jobs[i].mnemonic);
        }
    }
}

bool job_list_is_empty(void) { return _jobs_left == MAX_JOBS; }

Job *get_job(pid_t pid) {
    for (size_t i = 0; i < MAX_JOBS; i++) {
        if (_jobs[i].pid == pid) {
            return &_jobs[i];
        }
    }

    return NULL;
}

void decrease_job_list_count(void) {
    if (_jobs_left < MAX_JOBS) {
        _jobs_left++;
    }
}
