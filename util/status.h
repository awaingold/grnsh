#ifndef STATUS_H
#define STATUS_H

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

enum Status {
    RUNNING,
    DONE,
    STOPPED
};

struct job {
    pid_t pgid;
    char* text;
    enum Status status;
    int user_id;
    int num_processes;
    int num_exited;
};

int check_status_bg(struct job* jb);

#endif