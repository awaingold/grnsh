#ifndef BUILTINS_H
#define BUILTINS_H

void cd(char* argv[]);
int fg(int user_id);
int bg(int user_id);
int kill_job(int user_id);

#endif