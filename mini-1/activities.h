#ifndef ACTIVITIES_H
#define ACTIVITIES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define MAX_PROCESSES 100

typedef struct {
    pid_t pid;
    char command[256];
    int stopped; // 0: running, 1: stopped
} Process;

void add_process(pid_t pid, const char *command);
void update_process_status();
void fg_command(pid_t pid);
void activities();

#endif // ACTIVITIES_H