#ifndef STATUS_H
#define STATUS_H

#include "../util/job.h"

int check_status_bg(struct job* jb);
int check_status_fg(struct job* jb);

#endif