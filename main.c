#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "builtins.h"
#include "parser.h"

#define MAX_COMMANDS 8
#define MAX_TOKENS 8

int main() {

    size_t buffer_size = 0;
    ssize_t line_length;
    char* input = NULL;

    do {
        printf("> ");

        line_length = getline(&input, &buffer_size, stdin);

        if(line_length != -1) {
            if(input[0] != '\n') {
                char* argv[MAX_COMMANDS][MAX_TOKENS];
                char* saveptr;
                
                parse_command(strtok_r(input, "|", &saveptr), argv[0], MAX_TOKENS);
                int i = 1;
                while (i < MAX_COMMANDS) {
                    char* cmd = strtok_r(NULL, "|", &saveptr);
                    if(!cmd) {
                        break;
                    }
                    parse_command(cmd, argv[i], MAX_TOKENS);
                    i++;
                }

                int num_commands = i;

                if (strtok_r(NULL, "|", &saveptr)) {
                    printf("Error: too many commands\n");
                }

                printf("Num commands: %d\n", num_commands);

                // hardcoded piping test
                // if(strcmp(argv[0], "pipetest") == 0) {
                //     int pipefd[2];
                //     int pipe_status = pipe(pipefd);
                //     if (pipe_status < 0) {
                //         perror("grnsh");
                //     } else {
                //         pid_t id_1 = fork();
                //         if (id_1 < 0) {
                //             perror("grnsh");
                //         } else if (id_1 == 0) {
                //             // child (cmd1, ls)
                //             if (close(pipefd[0]) < 0) { // close read end
                //                 perror("grnsh");
                //             }
                //             if (dup2(pipefd[1], 1) < 0) { // replace write end
                //                 perror("grnsh");
                //             }
                //             char* argv[] = {"bad1", NULL};
                //             if (execvp(argv[0], argv) < 0) {
                //                 perror("grnsh");
                //                 _exit(1);
                //             }
                //         }

                //         pid_t id_2 = fork();
                //         if (id_2 < 0) {
                //             perror("grnsh");
                //         } else if (id_2 == 0) {
                //             // child (cmd2, wc -l)
                //             if (close(pipefd[1]) < 0) { // close write end
                //                 perror("grnsh");
                //             }
                //             if (dup2(pipefd[0], 0) < 0) { // replace read end
                //                 perror("grnsh");
                //             }
                //             char* argv[] = {"wc", "-l", NULL};
                //             if (execvp(argv[0], argv) < 0) {
                //                 perror("grnsh");
                //                 _exit(1);
                //             }
                            
                //         }

                //         close(pipefd[0]);
                //         close(pipefd[1]);
                //         int child_1_status;
                //         int child_2_status;
                //         waitpid(id_1, &child_1_status, 0);
                //         waitpid(id_2, &child_2_status, 0);
                //         continue;
    
                //     }
                // }

                // Check for empty, then for builtins
                if(argv[0][0] == NULL) {
                    continue;
                } else if (strcmp(argv[0][0], "cd") == 0) {
                    cd(argv[0]);
                } else if (strcmp(argv[0][0], "exit") == 0) {
                    break;
                } else {

                    pid_t p = fork();

                    
                    if (p < 0) { // Fork error
                        perror("grnsh");
                    } else if (p == 0) { // Child process
                        if (execvp(argv[0][0], argv[0]) < 0) { // Execvp error
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

