#include "log.h"

void logcmd(char* cmd, const char* log_file);
void writecmd(char* cmd, const char* log_file);
void purge(const char* log_file);
char* execute(int number, const char* log_file);
void handle_log(char *cmd_token, const char *log_file);

void purge(const char* log_file) {
    FILE *file = fopen(log_file, "w");
    if (file == NULL) {
        perror("fopen failed");
        return;
    }
    fclose(file);
}

void logcmd(char* cmd, const char* log_file) {
    if (strstr(cmd, "log") != NULL) {
        return;
    }

    static char last_cmd[1024] = "";
    if (strcmp(cmd, last_cmd) == 0) {
        return;
    }

    strcpy(last_cmd, cmd);
    writecmd(cmd, log_file);
}

void writecmd(char *cmd, const char *log_file) {
    FILE *file = fopen(log_file, "r");
    if (file == NULL) {
        perror("fopen failed");
        return;
    }

    char buffer[1024];
    int line_number = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        line_number++;
    }
    fclose(file);

    if (line_number >= MAX_LOG_SIZE) {
        FILE *file = fopen(log_file, "r");
        FILE *temp_file = fopen("temp.txt", "w");
        if (file == NULL || temp_file == NULL) {
            perror("fopen failed");
            return;
        }

        int line_number2 = 0;
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            line_number2++;
            if (line_number2 > 1) {
                fputs(buffer, temp_file);
            }
        }
        fprintf(temp_file, "%s\n", cmd);
        fclose(file);
        fclose(temp_file);

        remove(log_file);
        rename("temp.txt", log_file);
    } else {
        FILE *file = fopen(log_file, "a");
        if (file == NULL) {
            perror("fopen failed");
            return;
        }
        fprintf(file, "%s\n", cmd);
        fclose(file);
    }
}

void handle_log(char *cmd_token, const char *log_file) {
    char *command = strtok(cmd_token, " ");
    char *subcommand = strtok(NULL, " ");
    
    if (subcommand == NULL) {
        
        FILE *file = fopen(log_file, "r");
        if (file == NULL) {
            perror("fopen failed");
            return;
        }

        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            printf("%s", buffer);
        }
        fclose(file);
    } else if (strcmp(subcommand, "purge") == 0) {
        purge(log_file);
    
    } else {
        printf("Unknown log subcommand\n");
    }
}

char* execute(int number, const char* log_file) {
    FILE *file2 = fopen(log_file, "r");
    if (file2 == NULL) {
        perror("fopen failed");
        return NULL;
    }

    static char buffer[1024];
    int line_number = 0;

    while (fgets(buffer, sizeof(buffer), file2) != NULL) {
        line_number++;
    }
    fclose(file2);

    FILE *file = fopen(log_file, "r");
    if (file == NULL) {
        perror("fopen failed");
        return NULL;
    }
    number = line_number - number +1;
    line_number = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        line_number++;
        if (line_number == number) {
            fclose(file);
            return buffer;
        }
    }
    fclose(file);
    return NULL;
}