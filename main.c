#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "builtins.h"

int main() {

    size_t buffer_size = 0;
    ssize_t line_length;
    char* input = NULL;

    do {
        printf("> ");

        line_length = getline(&input, &buffer_size, stdin);

        if(line_length != -1) {
            if(input[0] != '\n') {
                char* argv[5];
                char* saveptr;
                argv[0] = strtok_r(input, " \n", &saveptr);
                int i = 1;
                while (i < 4) {
                    argv[i] = strtok_r(NULL, " \n", &saveptr);
                    if(!argv[i]) {
                        i++;
                        break;
                    }
                    i++;
                }

                argv[i] = NULL;

                if (strtok_r(NULL, " ", &saveptr)) {
                    printf("Error: too many tokens\n");
                }

                // Check for empty, then for builtins
                if(argv[0] == NULL) {
                    continue;
                } else if (strcmp(argv[0], "cd") == 0) {
                    cd(argv);
                } else if (strcmp(argv[0], "exit") == 0) {
                    break;
                } else {

                    pid_t p = fork();

                    
                    if (p < 0) { // Fork error
                        perror("grnsh");
                    } else if (p == 0) { // Child process
                        if (execvp(argv[0], argv) < 0) { // Execvp error
                            perror("grnsh");
                            _exit(1); // Need to exit child process immediately without flushing buffers
                        }
                    } else if (p > 0) { // Parent process
                        int status;
                        waitpid(p, &status, 0);
                        continue;
                    }
                }
            }
        } else {
            // Encountered an error if not at EOF or if ferror
            if (!feof(stdin) || ferror(stdin)) {
                perror("grnsh");
            }
        }
    } while (line_length != -1);

    free(input);
    return 0;
    
}

