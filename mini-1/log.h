#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LOG_SIZE 15
void logcmd(char* cmd, const char* log_file);
void writecmd(char* cmd, const char* log_file);
void purge(const char* log_file);
char* execute(int number, const char* log_file);
void handle_log(char *cmd_token, const char *log_file);