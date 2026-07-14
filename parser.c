#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#define DEFAULT_LENGTH 16
#define BUFFER_INCREMENT 16


/*
    Parses the input command into argv[] tokens
    @param input the raw command read from the client
    @param argv[] the token array
    @param max_tokens the max tokens allowed in input
    @throws error if tokens in input > max_tokens
*/
void parse_command(char* input, char* argv[], int max_tokens) {

    char* saveptr;

    argv[0] = strtok_r(input, " \n", &saveptr);

    int i = 1;

    while (i < max_tokens - 1) {
        argv[i] = strtok_r(NULL, " \n", &saveptr);
        if (!argv[i]) {
            i++;
            break;
        }
        i++;
    }

    argv[i] = NULL;

    if(strtok_r(NULL, " \n", &saveptr)) {
        fprintf(stderr, "Error: too many tokens\n");
    }
}

int is_delimiter(char c) {
    return (int)(c == '>' || c == '<' || c == ' ' || c == '\n');
}

int tokenize(char* input, char* argv[], int max_tokens) {
    char* buffer = malloc(DEFAULT_LENGTH);
    int argp = 0;
    int bufferp = 0;
    int bufferl = DEFAULT_LENGTH;
    for (int i = 0; i < strlen(input); i++) {
        if (is_delimiter(input[i]) || i == strlen(input) - 1) {
            if (argp >= max_tokens - 1) {
                fprintf(stderr, "Error: too many tokens.\n");
                free(buffer);
                return 1;
            }
            if (bufferp >= bufferl) {
                char* tmp = realloc(buffer, bufferl + BUFFER_INCREMENT);
                if (tmp) {
                    buffer = tmp;
                } else {
                    fprintf(stderr, "Error: memory error.\n");
                    free(buffer);
                    return -1;
                }
            }
            buffer[bufferp] = '\0';
            argv[argp] = malloc(sizeof(buffer));
            strcpy(argv[argp], buffer);
            argp++;
            if (is_delimiter(input[i])) {
                char delim[2];
                delim[0] = input[i];
                delim[1] = '\0';
                argv[argp] = malloc(sizeof(delim));
                strcpy(argv[argp], delim);
            }
            argp++;
            bufferp = 0;
            buffer = malloc(DEFAULT_LENGTH);
        } else {
            if (bufferp >= bufferl) {
                char* tmp = realloc(buffer, bufferl + BUFFER_INCREMENT);
                if (tmp) {
                    buffer = tmp;
                } else {
                    fprintf(stderr, "Error: memory error.\n");
                    free(buffer);
                    return -1;
                }
            }
            buffer[bufferp] = input[i];
            bufferp++;
        }
    }
    free(buffer);
    return 0;
}

