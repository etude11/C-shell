
#include "seek.h"

void colorprint(char *path, int is_dir)
{
    if (is_dir)
    {
        printf("\033[1;34m%s\033[0m\n", path);
    }
    else
    {
        printf("\033[1;32m%s\033[0m\n", path);
    }
}

int check_file(char *a, char *target)
{
    char a1[1024];
    strcpy(a1, a);
    if(strstr(a,".")){
        if(strcmp(a,target)==0){
            return 1;
        }
        return 0;
    }
    strcat(a1, ".");
    if (strncmp(a1, target, strlen(a1)) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void recurse(char *base_path, char *target, int search_files, int search_dirs, int *found_count, char *invoke, char *found_path)
{
    struct dirent *entry;
    char path[1024];
    struct stat info;

    DIR *dir = opendir(base_path);
    if (!dir)
        return;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);
        if (stat(path, &info) == 0)
        {
            if (S_ISDIR(info.st_mode) && search_dirs && strcmp(entry->d_name, target) == 0)
            {
                (*found_count)++;
                if (*found_count == 1) {
                    strcpy(found_path, path); // Store the path of the first found directory
                }
                colorprint(path, 1);
            }
            else if (S_ISREG(info.st_mode) && search_files && check_file(target, entry->d_name))
            {
                (*found_count)++;
                if (*found_count == 1) {
                    strcpy(found_path, path); // Store the path of the first found file
                }
                colorprint(path, 0);
            }

            if (S_ISDIR(info.st_mode))
            {
                recurse(path, target, search_files, search_dirs, found_count, invoke, found_path);
            }
        }
    }
    closedir(dir);
}


void seek(char *command_string)
{
    char *token;
    char *saveptr;
    char flags[1024] = "";
    char *search = NULL;
    char *target_dir = NULL;
    int search_files = 1;
    int search_dirs = 1;
    int execute_flag = 0;
    int found_count = 0;
    char invoke[1024];
    char found_path[1024] = ""; // New variable to store the found path
    getcwd(invoke, sizeof(invoke));

    token = strtok_r(command_string, " ", &saveptr);
    token = strtok_r(NULL, " ", &saveptr);

    while (token != NULL)
    {
        if (strstr(token, "-d") || strstr(token, "-f") || strstr(token, "-e"))
        {
            strcat(flags, token);
            token = strtok_r(NULL, " ", &saveptr);
        }
        else if (search == NULL)
        {
            search = strdup(token);
            token = strtok_r(NULL, " ", &saveptr);
        }
        else if (target_dir == NULL)
        {
            target_dir = strdup(token);
            token = strtok_r(NULL, " ", &saveptr);
        }
    }

    if (strstr(flags, "-d") && strstr(flags, "-f"))
    {
        printf("Invalid flags!\n");
        free(search);
        free(target_dir);
        return;
    }

    if (strstr(flags, "-d"))
        search_files = 0;
    if (strstr(flags, "-f"))
        search_dirs = 0;
    if (strstr(flags, "-e"))
        execute_flag = 1;

    char current_dir[1024];
    if (target_dir == NULL)
    {
        getcwd(current_dir, sizeof(current_dir));
        target_dir = current_dir;
    }

    recurse(target_dir, search, search_files, search_dirs, &found_count, invoke, found_path);

    if (found_count == 0)
    {
        printf("No match found!\n");
    }
    else if (found_count == 1 && execute_flag)
    {
        struct stat info;
        if (stat(found_path, &info) == 0)
        {
            if (S_ISREG(info.st_mode) && search_files)
            {
                FILE *file = fopen(found_path, "r");
                if (file)
                {
                    char c;
                    while ((c = fgetc(file)) != EOF)
                    {
                        putchar(c);
                    }
                    fclose(file);
                }
                else
                {
                    printf("Missing permissions for task!\n");
                }
            }
            else if (S_ISDIR(info.st_mode) && search_dirs)
            {
                if (chdir(found_path) == 0)
                {
                    printf("%s/\n", found_path + 2);
                }
                else
                {
                    printf("Missing permissions for task!\n");
                }
            }
        }
    }
}
