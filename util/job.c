#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include "../util/job.h"
#include "../src/grnsh.h"
#include <stdbool.h>

int next_id = 1;

/*
    Returns a pointer to the job with id user_id in the global job_table array.
    @param user_id the user-facing id of the requested job
    @returns a pointer to the job with the id user_id, or NULL if such a job doesn't exist.
*/
struct job* lookup_job(int user_id) {
    for (int i = 0; i < MAX_JOBS; ++i) {
        if (job_table[i].user_id == user_id) {
            return &job_table[i];
        }
    }
    return NULL;
}

/*
    Adds a new job to the table with the specified pgid, input string, and number of processes.
    @param pgid the pgid of the new job
    @param input the new jobs input string
    @param num_processes the number of processes in the new job
    @returns 0 on success, -1 on failure
*/
int add_job(pid_t pgid, char* input, int num_processes) {
    for (int i = 0; i < MAX_JOBS; ++i) {
        if (!job_table[i].in_use) {
            struct job jb;
            jb.pgid = pgid;
            jb.num_processes = num_processes;
            jb.num_exited = 0;
            jb.status = RUNNING;
            jb.user_id = next_id;
            jb.in_use = true;
            // printf("before text malloc\n");
            jb.text = malloc(strlen(input) + 1);
            // printf("before strcpy\n");
            strcpy(jb.text, input);
            next_id++;
            job_table[i] = jb;
            return 0;
        }
    }
    return -1;
}

/*
    Removes the job with the specified user id from the job table. Frees the text field of the job.
    @param user_id the id of the job to be removed
    @return 0 on success
*/
int remove_job(int user_id) {
    for (int i = 0; i < MAX_JOBS; ++i) {
        if (job_table[i].user_id == user_id) {
            free(job_table[i].text);
            job_table[i].in_use = false;
            return 0;
        }
    }
    return 0;
}

