#include "reveal.h"

void list(int hidden_flag, const char *dir_path, char *home, char *prevcd)
{
    if (strcmp(dir_path, "-") == 0)
    {
        dir_path = strdup(prevcd);
    }
    if (strcmp(dir_path, "~") == 0)
    {
        dir_path = strdup(home);
    }
    // printf("%s\n",dir_path);
    DIR *dir = opendir(dir_path);
    if (dir)
    {
        struct dirent *entry;
        struct stat sb;
        char full_path[1024];

        while ((entry = readdir(dir)) != NULL)
        {
            if (!hidden_flag && entry->d_name[0] == '.')
            {
                continue;
            }

            snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

            if (stat(full_path, &sb) == -1)
            {
                perror("stat");
                continue;
            }

            // Determine the type of file
            char *color;
            if (S_ISDIR(sb.st_mode))
            {
                color = BLUE; // Directories
            }
            else if (sb.st_mode & S_IXUSR)
            {
                color = GREEN; // Executables
            }
            else
            {
                color = WHITE; // Regular files
            }

            printf("%s%s%s\n", color, entry->d_name, RESET);
        }
        closedir(dir);
    }
    else
    {
        perror("opendir");
    }
    return;
}

void print_file_info(const char *name, const struct stat *sb)
{
    printf((S_ISDIR(sb->st_mode)) ? "d" : "-");
    printf((sb->st_mode & S_IRUSR) ? "r" : "-");
    printf((sb->st_mode & S_IWUSR) ? "w" : "-");
    printf((sb->st_mode & S_IXUSR) ? "x" : "-");
    printf((sb->st_mode & S_IRGRP) ? "r" : "-");
    printf((sb->st_mode & S_IWGRP) ? "w" : "-");
    printf((sb->st_mode & S_IXGRP) ? "x" : "-");
    printf((sb->st_mode & S_IROTH) ? "r" : "-");
    printf((sb->st_mode & S_IWOTH) ? "w" : "-");
    printf((sb->st_mode & S_IXOTH) ? "x" : "-");

    printf(" %lu", (unsigned long)sb->st_nlink);

    struct passwd *pw = getpwuid(sb->st_uid);
    struct group *gr = getgrgid(sb->st_gid);
    printf(" %s %s", pw ? pw->pw_name : "UNKNOWN", gr ? gr->gr_name : "UNKNOWN");

    printf(" %5ld", (long)sb->st_size);

    char timebuf[80];
    struct tm *timeinfo = localtime(&sb->st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", timeinfo);
    printf(" %s", timebuf);

    // Determine the color for the file
    char *color;
    if (S_ISDIR(sb->st_mode))
    {
        color = BLUE; // Directories
    }
    else if (sb->st_mode & S_IXUSR)
    {
        color = GREEN; // Executables
    }
    else
    {
        color = WHITE; // Regular files
    }

    printf(" %s%s%s\n", color, name, RESET);
}

void list_all(int hidden_flag, const char *dir_path, char *home, char *prevcd)
{
    if (strcmp(dir_path, "-") == 0)
    {
        dir_path = strdup(prevcd);
    }
    if (strcmp(dir_path, "~") == 0)
    {
        dir_path = strdup(home);
    }
    DIR *dir = opendir(dir_path);
    if (dir == NULL)
    {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    struct stat sb;
    char full_path[1024];

    while ((entry = readdir(dir)) != NULL)
    {
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        if (stat(full_path, &sb) == -1)
        {
            perror("stat");
            continue;
        }
        if (!hidden_flag && entry->d_name[0] == '.')
        {
            continue;
        }

        print_file_info(entry->d_name, &sb);
    }

    closedir(dir);
    return;
}

void exec_list(char *cmd, char *home, char *prevcd)
{
    char *path = ".";
    int hidden_flag = 0;
    int long_format = 0;
    char *token;
    char temp[1024];

    // Get the first token
    // printf("CMd: %s\n",cmd); 
    strcpy(temp,cmd);
    token = strtok(temp, " ");
    // token = strtok

    // Walk through other tokens
    while (token != NULL)
    {
        if (strncmp(token, "-", 1) == 0)
        {
            if (strstr(token, "a"))
            {
                hidden_flag = 1;
            }
            if (strstr(token, "l"))
            {
                long_format = 1;
            }
        }
        token = strtok(NULL, " "); // Get the next token
    }
    // printf("CMd: %s\n",cmd);    
    char *path_pos = strrchr(cmd, ' ');
    
    if (path_pos && *(path_pos + 1) != '-')
    {
        // printf("in there\n");
        path = path_pos + 1;
    }
    // printf("Path: %s\n",path);
    if (long_format)
    {
        printf("Listing for path: %s\n", path);
        list_all(hidden_flag, path, home, prevcd);
    }
    else
    {
        list(hidden_flag, path,home,prevcd);
    }
}
