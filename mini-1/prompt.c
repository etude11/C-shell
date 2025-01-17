#include "prompt.h"
#define RESET "\033[0m"

void prompt(char* home)
{
    printf("\n");
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    char username[256];
    char hostname[256];
    char cwd[1024];
    if (pw)
    {
        strcpy(username, pw->pw_name);
    }
    else
    {
        perror("getpwuid");
    }

    if (gethostname(hostname, sizeof(hostname)) == 0)
    {
    }
    else
    {
        perror("gethostname");
    }

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        
        if (strncmp(cwd, home, strlen(home)) == 0)
        {
            
            printf("<%s%s%s@","\033[34m",username,RESET);
            printf("%s%s%s:~%s%s%s> ","\033[32m",hostname,RESET,"\033[36m",cwd+strlen(home),RESET);
        }
        else
        {
            
            
            printf("<%s%s%s@","\033[34m",username,RESET);
            printf("%s%s%s:%s%s%s> ","\033[32m",hostname,RESET,"\033[36m",cwd,RESET);
        }
    }
    else
    {
        perror("getcwd");
        return ;
    }



}

