#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

void cd(char* argv[]) {
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

