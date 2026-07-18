#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

static void handle_sigint(int signal_id, siginfo_t *info, void *context) {
    printf("not exiting\n");
}

void setup() {
    struct sigaction sigint_act = { 0 };
    sigint_act.sa_sigaction = &handle_sigint;
    sigint_act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sigint_act, NULL);
}
