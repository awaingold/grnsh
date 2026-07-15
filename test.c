#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "builtins.h"
#include "parser.h"

void test_tokenize1() {
    char* argv[16];
    char* input = "verylongfilename.txt>cmd<bar.txt";
    tokenize(input, argv, 16);
    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    printf("%s\n", argv[2]);
    printf("%s\n", argv[3]);
    printf("%s\n", argv[4]);
    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);
    free(argv[4]);
}

void test_tokenize2() {
    char* argv[16];
    char* input = "> out.txt";
    tokenize(input, argv, 16);
    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    printf("%s\n", argv[2]);
    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
}

void test_tokenize3() {
    char* argv[16];
    char* input = "wc<>out.txt";
    tokenize(input, argv, 16);
    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    printf("%s\n", argv[2]);
    printf("%s\n", argv[3]);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);

}

void test_tokenize4() {
    char* argv[16];
    char* input = "ls > out.txt";
    tokenize(input, argv, 16);
    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    printf("%s\n", argv[2]);
    printf("%s\n", argv[3]);
    printf("%s\n", argv[4]);
    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);
    free(argv[4]);
}

void test_tokenize5() {
    char* argv[16];
    char* input = "echo hi >> out.txt";
    tokenize(input, argv, 16);
    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    printf("%s\n", argv[2]);
    printf("%s\n", argv[3]);
    printf("%s\n", argv[4]);
    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);
    free(argv[4]);
    free(argv[5]);
    free(argv[6]);
}
    

int main() {
    test_tokenize1();
    //test_tokenize2();
    test_tokenize3();
    test_tokenize4();
    test_tokenize5();
    return 0;
}


