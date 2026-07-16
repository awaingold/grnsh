#ifndef PARSER_H
#define PARSER_H

void parse_command(char* input, char* argv[], char** input_file, char** output_file, int* append_flag, int max_tokens);
int tokenize(char* input, char* argv[], int max_tokens);
int is_delimiter(char c);

#endif