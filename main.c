#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

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

                pid_t p = fork();

                if (p < 0) {
                    perror(strerror(errno));
                } else if (p == 0) {
                    if (execvp(argv[0], argv) < 0) {
                        perror(strerror(errno));
                        _exit(1);
                    }
                } else if (p > 0) {
                    int status;
                    waitpid(p, &status, 0);
                    continue;
                }
            }
        } else {
            perror(strerror(errno));
        }
    } while (line_length != -1);

    free(input);
    return 0;
    
}