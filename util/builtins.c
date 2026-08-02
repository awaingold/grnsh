#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "../util/job.h"
#include "../util/status.h"

/*
Changes the directory of the calling process to argv[1], or to the user's home directory if argv[1] is null.
@requires argv != NULL
@param argv[] contains the destination path in argv[1]
@throws error on chdir() fail
*/
void cd(char* argv[]) {
    if (argv == NULL) {
        fprintf(stderr, "Error: argv[] must be non-null.");
    }
    char* path;
    if (argv[1] == NULL) {
        path = getenv("HOME");
    } else {
        path = argv[1];
    }
    if (chdir(path) < 0) {
        perror("cd");
    }
}

/*
    Backgrounds the job with id user_id
    @param user_id the id to be backgrounded
    @return 0 on success, -1 on error
    @throws error on error, or if no job with id user_id exists.
*/
int bg(int user_id) {
    struct job* jb = lookup_job(user_id);
    if (jb == NULL) {
        fprintf(stderr, "grnsh: job %d not found.\n", user_id);
        return -1;
    }
    pid_t pgid = jb->pgid;
    if (jb->status != RUNNING) {
        if (kill(-pgid, SIGCONT) < 0) {
            perror("grnsh");
            return -1;
        }
        jb-> status = RUNNING;
    }
    char* text = jb->text;
    if (text[strlen(text) - 1] != '\n') {
        printf("[%d]+ %s\n", user_id, text);
    } else {
        printf("[%d]+ %s", user_id, text);
    }
    return 0;
}

/*
    Foregrounds the job with id user_id
    @param user_id the id to be foregrounded
    @return 0 on success, -1 on error
    @throws error on error, or if no job with id user_id exists.
*/
int fg(int user_id) {
    struct job* jb = lookup_job(user_id);
    if (jb == NULL) {
        fprintf(stderr, "grnsh: job %d not found.\n", user_id);
        return -1;
    }
    pid_t pgid = jb->pgid;
    if (jb->status != RUNNING) {
        if (kill(-pgid, SIGCONT) < 0) {
        perror("grnsh");
        return -1;
    }
        jb->status = RUNNING;
    }
    if (tcsetpgrp(STDIN_FILENO, pgid) < 0) {
        perror("grnsh");
        return -1;
    }
    if (check_status_fg(jb) < 0) {
        tcsetpgrp(STDIN_FILENO, getpgrp());
        return -1;
    }
    tcsetpgrp(STDIN_FILENO, getpgrp());
    return 0;
}

/*
    Terminates the job with id user_id
    @param user_id the id to be terminated
    @return 0 on success, -1 on error
    @throws error on error
*/
int kill_job(int user_id) {
    struct job* jb = lookup_job(user_id);
    if (jb == NULL) {
        fprintf(stderr, "grnsh: job %d not found.\n", user_id);
        return -1;
    }
    pid_t pgid = jb->pgid;
    if (kill(-pgid, SIGTERM) < 0) {
        perror("grnsh");
        return -1;
    }
    // need to wake up process so it can actually respond to SIGTERM
    if (kill(-pgid, SIGCONT) < 0) {
        perror("grnsh");
        return -1;
    }
    check_status_fg(jb);
    remove_job(user_id);
    return 0;
}

/*
    Prints a list of shell builtins.
*/
void help() {
    printf("grnsh (pronounced \"grinch\")\n");
    printf("available shell builtins:\n\n");
    printf("cd <path> \t changes the current working directory to path\n");
    printf("fg <job_id> \t moves the background job with id job_id to the foreground.\n");
    printf("bg <job_id> \t resumes the backgrounded job with id job_id\n");
    printf("kill <job_id> \t terminates the job with id job_id\n");
}