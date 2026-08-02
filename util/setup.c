#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

// This file has lots of red squiggles because VS code doesn't understand the stuff from signal.h
// But it still compiles without warnings!

/*
    Signal handler for SIGINT. Writes a newline to stdout.
*/
static void handle_sigint(int sig) {
    write(1, (void*)"\n> ",3);
}

/*
    Sets up the signal handler for SIGINT (ignores).
    @return 0 on success, -1 on error
    @throws error on error
*/
int setup_sigint() {
    struct sigaction act = { 0 };
    act.sa_handler = &handle_sigint;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("grnsh");
        return -1;
    }
    return 0;
}

/*
    Sets up the signal handler for SIGTSTP.
    @return 0 on success, -1 on error
    @throws error on error
*/
int setup_sigtstp() {
    struct sigaction act = { 0 };
    act.sa_handler = SIG_IGN;
    if (sigaction(SIGTSTP, &act, NULL) < 0) {
        perror("grnsh");
        return -1;
    }
    return 0;
}

/*
    Sets up the signal handler for SIGTTOU.
    @return 0 on success, -1 on error
    @throws error on error
*/
int setup_sigttou() {
    struct sigaction act = { 0 };
    act.sa_handler = SIG_IGN;
    if (sigaction(SIGTTOU, &act, NULL) < 0) {
        perror("grnsh");
        return -1;
    }
    return 0;
}

/*
    Sets up signal handling for the shell itself.
    @return 0 on success, -1 on error
*/
int setup() {
    if (setup_sigint() < 0) {
        return -1;
    }
    if (setup_sigtstp() < 0) {
        return -1;
    }
    if (setup_sigttou() < 0) {
        return -1;
    }
    return 0;
}

/*
    Resets the signal handler for SIGINT.
    @return 0 on success, -1 on error
    @throws error on error
*/
int reset_sigint() {
    struct sigaction act = { 0 };
    act.sa_handler = SIG_DFL;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("grnsh");
        return -1;
    }
    return 0;
}

/*
    Resets the signal handler for SIGTSTP.
    @return 0 on success, -1 on error
    @throws error on error
*/
int reset_sigtstp() {
    struct sigaction act = { 0 };
    act.sa_handler = SIG_DFL;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGTSTP, &act, NULL) < 0) {
        perror("grnsh");
        return -1;
    }
    return 0;
}

/*
    Resets signal handlers for SIGINT and SIGTSTP to their default values.
    @return 0 on success, -1 on error
*/
int reset_handlers() {
    if (reset_sigint() < 0) {
        return -1;
    }
    if (reset_sigtstp() < 0) {
        return -1;
    }
    return 0;
}