#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>


int ping(int pid, int sig);
void execping(char* cmd);

#endif

