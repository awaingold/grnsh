#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define DEFAULT_LENGTH 16
#define BUFFER_INCREMENT 16

/*
    Returns whether c is a delimiter character.
    @param c the character to test
    @returns 1 if c is a delimiter, 0 otherwise
*/
int is_delimiter(char c) {
    return (int)(c == '>' || c == '<' || c == ' ' || c == '\n');
}

/*
    Parses input into tokens with delimiters ' ', '<', '>', and '>>'. Delimiters are also tokenized.
    @param input the input to tokenize
    @param argv an array for the tokens to be placed into
    @param max_tokens the maximum number of tokens
    @requires len(argv) = max_tokens
    @throws error if too many tokens in input
    @returns 0 on success, -1 on error
*/
int tokenize(char* input, char* argv[], int max_tokens) {
    char* buffer = malloc(DEFAULT_LENGTH);
    int argp = 0;
    int bufferp = 0;
    int bufferl = DEFAULT_LENGTH;
    int len = strlen(input);
    for (int i = 0; i < len; i++) {
        if (is_delimiter(input[i]) || i == strlen(input) - 1) {
            if (argp >= max_tokens - 1) {
                fprintf(stderr, "Error: too many tokens.\n");
                free(buffer);
                return 1;
            }
            if (bufferp >= bufferl - 1) {
                char* tmp = realloc(buffer, bufferl + BUFFER_INCREMENT);
                if (tmp) {
                    buffer = tmp;
                    bufferl += BUFFER_INCREMENT;
                } else {
                    fprintf(stderr, "Error: memory error.\n");
                    free(buffer);
                    return -1;
                }
            }
            if (!is_delimiter(input[i])) {
                buffer[bufferp] = input[i];
                bufferp++;
            }
            buffer[bufferp] = '\0';
            argv[argp] = malloc(bufferp + 1);
            strcpy(argv[argp], buffer);
            argp++;
            if (is_delimiter(input[i])) {
                char delim[3];
                delim[0] = input[i];
                if (i < len - 1 && input[i] == '>') {
                    if (input[i + 1] == '>') {
                        delim[1] = '>';
                        delim[2] = '\0';
                        i++;
                        argv[argp] = malloc(sizeof(delim));
                    } else {
                        delim[1] = '\0';
                        argv[argp] = malloc(sizeof(delim) - sizeof(char));
                    }
                } else {
                    delim[1] = '\0';
                    argv[argp] = malloc(sizeof(delim) - sizeof(char));
                }
                
                strcpy(argv[argp], delim);
                argp++;
            } else {
                argv[argp] = NULL;
            }
            
            bufferp = 0;
            free(buffer);
            buffer = malloc(DEFAULT_LENGTH);
        } else {
            if (bufferp >= bufferl) {
                char* tmp = realloc(buffer, bufferl + BUFFER_INCREMENT);
                if (tmp) {
                    buffer = tmp;
                    bufferl += BUFFER_INCREMENT;
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
    while (argp < max_tokens) {
        argv[argp] = NULL;
        argp++;
    }
    return 0;
}

/*
    Parses the input command into argv[] tokens
    @param input the raw command read from the client
    @param argv[] the token array
    @param max_tokens the max tokens allowed in input
    @throws error if tokens in input > max_tokens
*/
void parse_command(char* input, char* argv[], char** input_file, char** output_file, int* append_flag, int max_tokens) {
    char* tokens[max_tokens];
    if (tokenize(input, tokens, max_tokens) < 0) {
        return;
    }
    *input_file = NULL;
    *output_file = NULL;
    *append_flag = 0;
    int argv_ptr = 0;
    for (int i = 0; i < max_tokens; ++i) {
        if (tokens[i] == NULL) {
            break;
        }
        if (strcmp(tokens[i], " ") != 0 && strcmp(tokens[i], "") != 0 && strcmp(tokens[i], "\n") != 0) {
            if (strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], ">>") == 0) {
                if (strcmp(tokens[i], ">>") == 0) {
                    *append_flag  = 1;
                }
                while (i < max_tokens - 1 && (strcmp(tokens[i + 1], " ") == 0 || strcmp(tokens[i + 1], "") == 0)) {
                    free(tokens[i]);
                    ++i;
                }
                if (i < max_tokens - 1) {
                    free(tokens[i]);
                    *output_file = tokens[i + 1];
                    ++i;
                }
            } else if (strcmp(tokens[i], "<") == 0) {
                while (i < max_tokens - 1 && (strcmp(tokens[i + 1], " ") == 0 || strcmp(tokens[i + 1], "") == 0)) {
                    free(tokens[i]);
                    ++i;
                }
                if (i < max_tokens - 1) {
                    free(tokens[i]);
                    *input_file = tokens[i + 1];
                    ++i;
                }
            } else {
                argv[argv_ptr] = malloc(strlen(tokens[i]) + 1);
                strcpy(argv[argv_ptr], tokens[i]);
                argv_ptr++;
                free(tokens[i]);
            }
        } else {
            free(tokens[i]);
        }
    }
    if (argv_ptr > max_tokens - 1) {
        perror("Too many tokens");
    } else {
        argv[argv_ptr] = NULL;
    }
}

