#include "hop.h"


char* hop(char* argum,char* home,char* pevcwd)
{
    char *token;
    char cwd[1024];
    char *saveptr;
    char path[1024];
    if(strcmp(argum,"hop")==0){
        chdir(home);
         if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("%s\n", cwd);
        }
        else
        {
            perror("getcwd");
        }
    }
    token = strtok_r(argum, " ", &saveptr);
    token = strtok_r(NULL, " ", &saveptr);
    while (token != NULL)
    {

        if (strcmp(token, "-") == 0)
        {
            strcpy(path, pevcwd);
        }
        else if(strcmp(token,"~")==0){
            strcpy(path,home);
        }
        else{
            strcpy(path,token);
        }

        if (getcwd(pevcwd, sizeof(cwd)) != NULL)
        {
        }
        if (chdir(path) == -1)
        {
            perror("chdir failed");
            return pevcwd;
        }

        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("%s\n", cwd);
        }
        else
        {
            perror("getcwd");
        }
        token = strtok_r(NULL, " ", &saveptr);
    }
    return pevcwd;
}