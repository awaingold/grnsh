#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

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

