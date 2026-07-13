#ifndef PARSER_H
#define PARSER_H

void parse_command(char* input, char* argv[], int max_tokens);
void tokenize(char* input, char* argv[]);

#endif