#ifndef JOB_H
#define JOB_H

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_JOBS 16

enum Status {
    RUNNING,
    DONE,
    STOPPED
};

struct job {
    char* text;
    pid_t pgid;
    enum Status status;
    int user_id;
    int num_processes;
    int num_exited;
    bool in_use;
};

int add_job(pid_t pgid, char* input, int num_processes, bool running);
int remove_job(int user_id);
struct job* lookup_job(int user_id);

#endif