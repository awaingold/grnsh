#include <stdlib.h>

/*
    Frees every element of the null-terminated array argv.
    @param argv the array to be freed
    @param max_tokens the size of argv
    @requires argv is null-terminated
    @return -1 on error, 0 on success
*/
int cleanup(char* argv[], int max_tokens) {
    if (!argv) {
        return -1;
    }
    for (int k = 0; k < max_tokens; k++) {
        if (argv[k] != NULL) {
        free(argv[k]);
    } else {
        break;
        }
    }
    return 0;
}