#ifndef SPEC7_H
#define SPEC7_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

// Function to retrieve and print process information
void get_process_info(pid_t pid);

// Function to parse command and print process info based on PID
void print_proc(char *cmd);

#endif // SPEC7_H
