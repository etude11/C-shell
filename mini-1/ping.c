#include "ping.h"
int ping(int pid,int sig){
    pid_t pi = (pid_t)pid;
    if (kill(pi, sig) == -1) {
        printf("No such process found\n");
        return 0;

    }

    
    printf("Signal %d sent to process %d\n", sig, pid);
    return 0;
}

void execping(char* cmd){
    char* command = strtok(cmd," ");
    char* arg1 = strtok(NULL," ");
    char* arg2 = strtok(NULL," ");
    ping(atoi(arg1),atoi(arg2));
}