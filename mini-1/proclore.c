#include "proclore.h"

void get_process_info(pid_t pid)
{
    char path[256];
    char buffer[256];
    FILE *file;

    printf("pid: %d\n", pid);

    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    file = fopen(path, "r");
    if (!file)
    {
        printf("Process with pid : %d doesn't exists.\n",pid);
        return;
    }

    while (fgets(buffer, sizeof(buffer), file))
    {
        if (strncmp(buffer, "State:", strlen("State:")) == 0)
        {
            printf("process status : %s", buffer + 7);
        }
        else if (strncmp(buffer, "Tgid:", strlen("Tgid:")) == 0)
        {
            printf("Process Group : %s", buffer + 6);
        }
        else if (strncmp(buffer, "VmSize:", 7) == 0)
        {
            printf("Virtual memory : %s", buffer + 8);
        }
    }
    fclose(file);

    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len != -1)
    {
        buffer[len] = '\0';
        printf("executable path : %s\n", buffer);
    }
    else
    {
        perror("readlink");
    }
}

void print_proc(char *cmd)
{

    char *token;
    char *saveptr;
    token = strtok_r(cmd, " ", &saveptr);
    token = strtok_r(NULL, " ", &saveptr);

    if(token!=NULL){

        pid_t pid = (pid_t)atoi(token);
        get_process_info(pid);

    }
    else{
        pid_t pid = getpid();
        get_process_info(pid);
    }
}
