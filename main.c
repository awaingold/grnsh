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

                char* input_filepaths[MAX_COMMANDS];
                char* output_filepaths[MAX_COMMANDS];
                int flags[MAX_COMMANDS];
                
                parse_command(strtok_r(input, "|", &saveptr), argv[0], &input_filepaths[0], &output_filepaths[0], &flags[0], MAX_TOKENS);
                int i = 1;
                while (i < MAX_COMMANDS) {
                    char* cmd = strtok_r(NULL, "|", &saveptr);
                    if(!cmd) {
                        break;
                    }
                    parse_command(cmd, argv[i], &input_filepaths[i], &output_filepaths[i], &flags[i], MAX_TOKENS);
                    i++;
                }

                int N = i;

                if (strtok_r(NULL, "|", &saveptr)) {
                    printf("Error: too many commands\n");
                }

                // Check for empty, then for builtins
                if(argv[0][0] == NULL) {
                    continue;
                } else if (strcmp(argv[0][0], "cd") == 0) {
                    cd(argv[0]);
                } else if (strcmp(argv[0][0], "exit") == 0) {
                    break;
                } else { // piping logic

                    int pipefd[N - 1][2];
                    pid_t ids[N];
                    int pipes_created = 0;

                    for (int k = 0; k < N - 1; k++) {
                        if (pipe(pipefd[k]) < 0) {
                            perror("grnsh");
                        } else {
                            pipes_created++;
                        }
                    }

                    if (pipes_created != N - 1) {
                        continue;
                    }
                
                    for (int k = 0; k < N; k++) {
                        pid_t pid = fork();
                        if (pid < 0) {
                            //error
                            perror("grnsh");
                            N = k;
                            break;
                        } else if (pid == 0) {
                            // child
                            if (k != 0) {
                                if (dup2(pipefd[k - 1][0], 0) < 0) {
                                    perror("grnsh");
                                    _exit(1);
                                }
                            }
                            if (k != N - 1) {
                                if (dup2(pipefd[k][1], 1) < 0) {
                                    perror("grnsh");
                                    _exit(1);
                                }
                            }
                            for (int i = 0; i < N - 1; i++) { // close ALL pipe ends to avoid hanging
                                close(pipefd[i][0]);
                                close(pipefd[i][1]);
                            }
                            if (execvp(argv[k][0], argv[k]) < 0) {
                                perror("grnsh");
                                _exit(1);
                            } 
                        } else {
                            // parent
                            ids[k] = pid;
                            }
                    }
        

                    for (int i = 0; i < pipes_created; i++) { // close ALL pipe ends to avoid hanging, but in parent this time
                        close(pipefd[i][0]);
                        close(pipefd[i][1]);
                    }
                    int statuses[N];
                    for (int k = 0; k < N; k++) {
                        waitpid(ids[k], &statuses[k], 0);
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

