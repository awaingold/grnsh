#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include "../util/job.h"
#include "../src/grnsh.h"

/*
    Helper. Checks the status of the processes in jb with the given flags
    @param jb the job to be checked
    @param flags the flags to check with
    @return 0 on success, -1 on error.
    @requires jb != NULL
*/
int check_status(struct job* jb, int flags) {
    pid_t pid = -1;
    int status;
    while (pid != 0) {
        pid = waitpid(-jb->pgid, &status, flags);
        if (pid < 0) {
            if (errno == ECHILD) {
                jb->num_exited = jb->num_processes;
                jb->status = DONE;
                return 0;
            } else {
                perror("grnsh");
                return -1;
            }
        } else if (pid > 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                jb->num_exited++;
            } else if (WIFSTOPPED(status)) {
                jb->status = STOPPED;
                return 0;
            }
        }
    }
    if (jb->num_exited == jb->num_processes) {
        jb->status = DONE;
    }
    return 0;
}

/*
    Checks the status of the BACKGROUND job pointed to by jb and updates its status accordingly.
    @param jb the pointer to the struct job to be checked
    @return 0 on success, -1 on error
    @requires jb != NULL
    @requires jb is a background job
*/
int check_status_bg(struct job* jb) {
    return check_status(jb, WUNTRACED | WNOHANG);
}

/*
    Checks the status of the FOREGROUND job pointed to by jb and updates its status accordingly.
    @param jb the pointer to the struct job to be checked
    @return 0 on success, -1 on error
    @requires jb != NULL
    @requires jb is a foreground job
*/
int check_status_fg(struct job* jb) {
    return check_status(jb, WUNTRACED);
}

/*
    Checks the status of all jobs in the job table, and prints the finished jobs to the standard output.
    Removes the finished jobs from the job table.
    @return 0 on success, -1 on failure.
*/
int check_all_jobs() {
    for (int i = 0; i < MAX_JOBS; ++i) {
        if (job_table[i].in_use) {
            check_status_bg(&job_table[i]);
            if (job_table[i].status == DONE) {
                printf("[%d] Done %s\n", job_table[i].user_id, job_table[i].text);
                remove_job(job_table[i].user_id);
            }
        }
    }
    return 0;
}