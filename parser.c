#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>


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