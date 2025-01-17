// shell.h
#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>     // For open() and file control options
#include <sys/stat.h>  // For file mode constants
#include <ctype.h>
#include <signal.h>
#include "prompt.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "proclore.h"
#include "seek.h"
#include "activities.h"
#include "ping.h"
#include "iman.h"
#include "neonate.h"

// Function Prototypes

// Check if a command is valid either as a built-in or available in the PATH
int is_command_valid(const char *command);

// Extracts the part of the string before the first whitespace character
void get_part_before_whitespace(const char *str, char *result);

// Trims leading and trailing whitespace from the string
void trim_whitespace(char *str);

// Counts occurrences of a specific character in a string
int count_char_occurrences(const char *str, char ch);

// Handles the SIGCHLD signal to clean up zombie processes
void handle_sigchld(int sig);

// Executes the parsed command string
void executecmd(char *cmd, char *home, char *prevwd);


#endif // SHELL_H
