#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include "../util/builtins.h"
#include "../util/parser.h"
#include "../util/cleanup.h"
#include "../util/setup.h"
#include "../util/job.h"
#include "../util/status.h"

#define MAX_COMMANDS 32
#define MAX_TOKENS 512

struct job job_table[MAX_JOBS];

int main() {

    if (setup() < 0) {
        return 1;
    }

    bool is_interactive_session = false;
    if (isatty(STDIN_FILENO)) {
        is_interactive_session = true;
    }

    size_t buffer_size = 0;
    ssize_t line_length;
    char* input = NULL;

    do {
        if (check_all_jobs() < 0) {
            perror("grnsh");
        }
        printf("> ");

        line_length = getline(&input, &buffer_size, stdin);

        if(line_length != -1) {
            if(input[0] != '\n') {
                char* argv[MAX_COMMANDS][MAX_TOKENS];
                char* saveptr;

                char* input_filepaths[MAX_COMMANDS];
                char* output_filepaths[MAX_COMMANDS];
                int flags[MAX_COMMANDS];

                bool is_bg = parse_for_background(input);
                char* input_copy;
                input_copy = strdup(input);
                
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

                int num_commands= i;

                if (strtok_r(NULL, "|", &saveptr)) {
                    printf("Error: too many commands\n");
                }

                // Check for empty, then for builtins
    
                if(argv[0][0] == NULL) {
                    continue;
                } else if (strcmp(argv[0][0], "cd") == 0) {
                    cd(argv[0]);
                    for (int i = 0; i < num_commands; ++i) {
                        cleanup(argv[i], MAX_TOKENS);
                    }
                } else if (strcmp(argv[0][0], "exit") == 0) {
                    for (int i = 0; i < num_commands; ++i) {
                        cleanup(argv[i], MAX_TOKENS);
                    }
                    break;
                } else if (strcmp(argv[0][0], "fg") == 0) {
                    if (argv[0][1]) {
                        fg(atoi(argv[0][1]));
                    } else {
                        fprintf(stderr, "fg: expected 1 argument but got 0\n");
                    }
                    for (int i = 0; i < num_commands; ++i) {
                        cleanup(argv[i], MAX_TOKENS);
                    }
                } else if (strcmp(argv[0][0], "bg") == 0) {
                    if (argv[0][1]) {
                        bg(atoi(argv[0][1]));
                    } else {
                        fprintf(stderr, "bg: expected 1 argument but got 0\n");
                    }
                    for (int i = 0; i < num_commands; ++i) {
                        cleanup(argv[i], MAX_TOKENS);
                    }
                } else if (strcmp(argv[0][0], "kill") == 0) {
                    if (argv[0][1]) {
                        kill_job(atoi(argv[0][1]));
                    } else {
                        fprintf(stderr, "kill: expected 1 argument but got 0\n");
                    }
                    for (int i = 0; i < num_commands; ++i) {
                        cleanup(argv[i], MAX_TOKENS);
                    }
                } else { // piping logic

                    int pipefd[num_commands - 1][2];
                    int pipes_created = 0;

                    for (int k = 0; k < num_commands - 1; k++) {
                        if (pipe(pipefd[k]) < 0) {
                            perror("grnsh");
                        } else {
                            pipes_created++;
                        }
                    }

                    if (pipes_created !=num_commands- 1) {
                        continue;
                    }

                    pid_t pgid;
                
                    for (int k = 0; k < num_commands; k++) {
                        pid_t pid = fork();
                        if (pid < 0) {
                            //error
                            perror("grnsh");
                           num_commands= k;
                            break;
                        } else if (pid == 0) {
                            // child
                            if (reset_handlers() < 0) {
                                _exit(1);
                            }
                            if (is_interactive_session) {
                                if (k == 0) {
                                    if (setpgid(0, 0) < 0) {
                                        perror("grnsh");
                                        _exit(1);
                                    }
                                } else {
                                    if (setpgid(0, pgid) < 0) {
                                        perror("grnsh");
                                        _exit(1);
                                    }
                                }
                            }
                            if (k != 0) {
                                if (dup2(pipefd[k - 1][0], 0) < 0) {
                                    perror("grnsh");
                                    _exit(1);
                                }
                            }
                            if (k !=num_commands- 1) {
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
                                int fd;
                                if (flags[k]) {
                                    fd = open(output_filepaths[k], O_WRONLY | O_APPEND | O_CREAT, (mode_t)00700);
                                } else {
                                    fd = open(output_filepaths[k], O_WRONLY | O_CREAT | O_TRUNC, (mode_t)00700);
                                }
                                if (fd < 0) {
                                    perror("grnsh");
                                    _exit(1);
                                }
                                if(dup2(fd, 1) < 0) {
                                    perror("grnsh");
                                    _exit(1);
                                }
                                free(output_filepaths[k]);
                            }
                            for (int i = 0; i <num_commands- 1; i++) { // close ALL pipe ends to avoid hanging
                                close(pipefd[i][0]);
                                close(pipefd[i][1]);
                            }
                            if (execvp(argv[k][0], argv[k]) < 0) {
                                perror("grnsh");
                                _exit(1);
                            } 
                        } else {
                            // parent
                            if (is_interactive_session) {
                                if (k == 0) {
                                    if (setpgid(pid, pid) < 0) {
                                        perror("grnsh");
                                    }
                                    pgid = pid;
                                } else {
                                    if (setpgid(pid, pgid) < 0) {
                                        perror("grnsh");
                                    }
                                }
                            }
                            
                        }
                    }

                    if (!is_bg && is_interactive_session) {
                        if (tcsetpgrp(STDIN_FILENO, pgid) < 0) {
                            perror("grnsh");
                        }
                    }

                    for (int i = 0; i < pipes_created; i++) { // close ALL pipe ends to avoid hanging, but in parent this time
                        close(pipefd[i][0]);
                        close(pipefd[i][1]);
                    }
                    for (int k = 0; k < i; ++k) {
                        if (cleanup(argv[k], MAX_TOKENS) < 0) {
                            perror("grnsh");
                        }
                        if (input_filepaths[k]) {
                            free(input_filepaths[k]);
                        }
                        if (output_filepaths[k]) {
                            free(output_filepaths[k]);
                        }
                    }
                    if(!is_bg) {
                        // Horrible ugly way of doing this. Memory doesn't grow on trees, ya know!
                        struct job new_job = { 0 };
                        new_job.in_use = true;
                        new_job.num_exited = 0;
                        new_job.num_processes = num_commands;
                        new_job.pgid = pgid;
                        new_job.status = RUNNING;
                        new_job.text = input_copy;
                        if (check_status_fg(&new_job) < 0) {
                            perror("grnsh");
                        }
                        if (new_job.status == STOPPED) {
                            int user_id = add_job(pgid, input_copy, num_commands, false);
                            printf("[%d]+ Stopped %s", user_id, input_copy);
                        }
                        if (is_interactive_session) {
                                if (tcsetpgrp(STDIN_FILENO, getpgrp()) < 0) {
                                perror("grnsh");
                            }   
                        }
                    } else {
                        // Creating the new job in the background (running!)
                        int user_id = add_job(pgid, input_copy, num_commands, true);
                        if (user_id < 0) {
                            perror("grnsh");
                        } else {
                            printf("[%d] %d\n", user_id, pgid);
                        }
                        
                    }
                    free(input_copy);
                    
                    
                }
            }
        } else {
            // If interrupted, just continue (we want to ignore ctrl+c sigint)
            if (ferror(stdin) && errno == EINTR) {
                clearerr(stdin);
                continue;
            }
            // Encountered an error if not at EOF or if ferror
            if (!feof(stdin) || ferror(stdin)) {

                perror("grnsh");
            }
        }
    } while (line_length != -1);

    free(input);
    return 0;
    
}