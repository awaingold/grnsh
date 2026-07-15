#define DEFAULT_LENGTH_DELIM 4
#define INPUT_FLAG 1
#define SPACE_FLAG 0
#define OUTPUT_FLAG_WRITE -1
#define OUTPUT_FLAG_APPEND -2
#include <string.h>
#include "parser.h"

/*
    Consumes delimiters starting at index delim_index until encountering a non-delimiter character, placing the delimiter token into delimiter_token.
    @param input the input string
    @param delim_index the index of the first delimiter
    @param delimiter_token the delimiter token
    @requires delimiter_token is uninitialized
    @requries delim_index is valid
*/
int consume_delimiters(char* input, int delim_index, char delimiter_token[DEFAULT_LENGTH_DELIM]) {
    int delimp = 0;
    int len = strlen(input);
    int delim_flag = SPACE_FLAG;
    while (delim_index < len && is_delimiter(input[delim_index])) {
        if (input[delim_index] == '<') {
            delim_flag += INPUT_FLAG;
        } else if (input[delim_index] == '>') {
            delim_flag += OUTPUT_FLAG_WRITE;
        }
        delimiter_token[delimp] = input[delim_index];
        delim_index++;
        delimp++;
    }
    return 0;
}