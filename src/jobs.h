#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct {
    int32_t id;
    pid_t pid;
    char *mnemonic;
} Job;

int32_t add_job(pid_t pid, char *mnemonic);
int32_t bring_to_foreground(int32_t id);
void print_jobs(void);
bool job_list_is_empty(void);
Job *get_job(pid_t pid);
void decrease_job_list_count(void);
