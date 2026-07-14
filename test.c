#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "builtins.h"
#include "parser.h"

void test_tokenize() {
    char* argv[16];
    char* input = "foo.txt>cmd<bar.txt\n";
    tokenize(input, argv, 64);
    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    printf("%s\n", argv[2]);
    printf("%s\n", argv[3]);
    printf("%s\n", argv[4]);
}
    

int main() {
    test_tokenize();
    return 0;
}


