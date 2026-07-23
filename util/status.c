#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <../util/status.h>

/*
    Checks the status of the job pointed to by jb and updates its status accordingly.
    @param jb the pointer to the struct job to be checked
    @return 0 on success, -1 on error
    @throws error on error
    @requires jb != NULL
*/
int check_status_bg(struct job* jb) {
    pid_t pid = -1;
    int status;
    while (pid != 0) {
        pid = waitpid(-jb->pgid, &status, WUNTRACED | WNOHANG);
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