#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include "../util/builtins.h"
#include "../util/parser.h"
#include "../util/cleanup.h"
#include "./setup.h"

#define MAX_COMMANDS 32
#define MAX_TOKENS 512

int main() {

    if (setup() < 0) {
        return 1;
    }

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
                    for (int i = 0; i < N; ++i) {
                        cleanup(argv[i], MAX_TOKENS);
                    }
                } else if (strcmp(argv[0][0], "exit") == 0) {
                    for (int i = 0; i < N; ++i) {
                        cleanup(argv[i], MAX_TOKENS);
                    }
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
                            if (input_filepaths[k]) {
                                int fd = open(input_filepaths[k], O_RDONLY);
                                if (fd == -1) {
                                    perror("grnsh");
                                    _exit(1);
                                }
                                if (dup2(fd, 0) < 0) {
                                    perror("grnsh");
                                    _exit(1);
                                }
                                free(input_filepaths[k]);
                            }
                            if (output_filepaths[k]) {
                                // TODO: dup2()
                                int fd;
                                if (flags[k]) {
                                    fd = open(output_filepaths[k], O_WRONLY | O_APPEND | O_CREAT, (mode_t)00700);
                                } else {
                                    fd = open(output_filepaths[k], O_WRONLY | O_CREAT | O_TRUNC, (mode_t)00700);
                                }
                                if (fd == -1) {
                                    perror("grnsh");
                                    _exit(1);
                                }
                                if(dup2(fd, 1) < 0) {
                                    perror("grnsh");
                                    _exit(1);
                                }
                                free(output_filepaths[k]);
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
                    for (int k = 0; k < i; ++k) {
                        cleanup(argv[k], MAX_TOKENS);
                        if (input_filepaths[k]) {
                            free(input_filepaths[k]);
                        }
                        if (output_filepaths[k]) {
                            free(output_filepaths[k]);
                        }
                    }
                    int statuses[N];
                    for (int k = 0; k < N; k++) {
                        waitpid(ids[k], &statuses[k], 0);
                    }
                }
            }
        } else {
            // If interrupted, just continue (we want to ignore ctrl+c sigint)
            if (ferror(stdin) && errno == EINTR) {
                continue;
            }
            // Encountered an error if not at EOF or if ferror
            if (!feof(stdin) || ferror(stdin)) {
                printf("here error = %d\n", ferror(stdin));
                perror("grnsh");
            }
        }
    } while (line_length != -1);

    free(input);
    return 0;
    
}