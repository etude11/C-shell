#include "shell.h"
#include <signal.h>
#include <setjmp.h>

static volatile sig_atomic_t ctrl_c_pressed = 0;
static volatile sig_atomic_t ctrl_z_pressed = 0;
static pid_t foreground_pid = 0;

// Signal handlers
void handle_sigint(int sig)
{
    ctrl_c_pressed = 1;
    if (foreground_pid != 0)
    {
        kill(foreground_pid, SIGINT);
    }
    else
    {
        printf("\n");
        fflush(stdout);
    }
}

void handle_sigtstp(int sig)
{
    ctrl_z_pressed = 1;
    if (foreground_pid != 0)
    {
        kill(foreground_pid, SIGTSTP);
    }
}

void handle_sigchld(int sig)
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (WIFEXITED(status))
        {
            printf("\nBackground process %d exited with status %d\n", pid, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("\nBackground process %d terminated by signal %d\n", pid, WTERMSIG(status));
        }
        else if (WIFSTOPPED(status))
        {
            printf("\nProcess %d stopped\n", pid);
            // Add the stopped process to your background processes list here
            add_process(pid, "Unknown"); // You'll need to implement a way to store the command
        }
        fflush(stdout);
    }
}

int is_command_valid(const char *command)
{

    if (strcmp(command, "hop") == 0 || strcmp(command, "log") == 0 || strcmp(command, "reveal") == 0 || strcmp(command, "seek") == 0 || strcmp(command, "proclore") == 0 || strcmp(command, "activities") == 0 || strcmp(command, "iMan") == 0 || strcmp(command, "neonate") == 0 || strcmp(command, "bg") == 0 || strcmp(command, "fg") == 0)
        return 1;
    char *path_env = getenv("PATH");
    if (path_env == NULL)
    {
        return 0;
    }
    char path_copy[1024];
    strncpy(path_copy, path_env, sizeof(path_copy));
    path_copy[sizeof(path_copy) - 1] = '\0';
    char *path_dir = strtok(path_copy, ":");
    while (path_dir != NULL)
    {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path_dir, command);
        if (access(full_path, X_OK) == 0)
        {
            return 1;
        }

        path_dir = strtok(NULL, ":");
    }

    return 0;
}

void get_part_before_whitespace(const char *str, char *result)
{
    // Find the first whitespace in the input string
    const char *first_space = strchr(str, ' ');

    // If there's no whitespace, copy the entire string
    if (first_space == NULL)
    {
        strcpy(result, str);
    }
    else
    {
        // Calculate the number of characters to copy
        size_t length = first_space - str;

        // Copy the substring into the result buffer
        strncpy(result, str, length);

        // Null-terminate the result string
        result[length] = '\0';
    }
}

void trim_whitespace(char *str)
{
    char *start = str;
    char *end;

    while (isspace((unsigned char)*start))
    {
        start++;
    }

    if (*start == '\0')
    {
        *str = '\0';
        return;
    }

    end = start + strlen(start) - 1;

    while (end > start && isspace((unsigned char)*end))
    {
        end--;
    }

    *(end + 1) = '\0';

    memmove(str, start, end - start + 2);
}

int count_char_occurrences(const char *str, char ch)
{
    int count = 0;
    while (*str != '\0')
    {
        if (*str == ch)
        {
            count++;
        }
        str++;
    }
    return count;
}

// void handle_sigchld(int sig)
// {
//     int status;
//     pid_t pid;

//     while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
//     {
//         if (WIFEXITED(status))
//         {
//             printf("Background process %d exited with status %d\n", pid, WEXITSTATUS(status));
//         }
//         else if (WIFSIGNALED(status))
//         {
//             printf("Background process %d terminated by signal %d\n", pid, WTERMSIG(status));
//         }
//     }
// }

void executecmd(char *cmd, char *home, char *prevwd)
{
    char log_file[1024];
    int log_flag = 0;

    snprintf(log_file, sizeof(log_file), "%s/%s", home, "log.txt");
    char *saveptr;
    // char path[1024];
    char *cmd_token2;
    cmd_token2 = strtok_r(cmd, ";", &saveptr);

    while (cmd_token2 != NULL)
    {
        if (strstr(cmd_token2, "|") != NULL)
        {
            char *saveptr;
            char *cmd_token = strtok_r(cmd_token2, "|", &saveptr);
            char *commands[1024];
            int cmd_count = 0;

            // Store all pipe-separated commands
            while (cmd_token != NULL)
            {
                trim_whitespace(cmd_token);
                if (strlen(cmd_token) == 0)
                {
                    printf("Error: Invalid use of pipe\n");
                    return;
                }
                commands[cmd_count++] = cmd_token;
                cmd_token = strtok_r(NULL, "|", &saveptr);
            }

            int pipe_fds[2], in_fd = 0;

            for (int i = 0; i < cmd_count; i++)
            {
                pipe(pipe_fds);

                // Handle redirection for each command in the pipe
                char *input_file = NULL;
                char *output_file = NULL;
                int append = 0;
                int redirect_input = 0, redirect_output = 0;

                // Check for input redirection (only for the first command)
                if (i == 0)
                {
                    char *redir = strchr(commands[i], '<');
                    if (redir)
                    {
                        redirect_input = 1;
                        input_file = strtok(redir + 1, " ");
                        *redir = '\0';
                        trim_whitespace(input_file);
                        trim_whitespace(commands[i]);
                    }
                }

                // Check for output redirection (only for the last command)
                if (i == cmd_count - 1)
                {
                    char *redir = strchr(commands[i], '>');
                    if (redir)
                    {
                        if (*(redir + 1) == '>')
                        {
                            append = 1;
                            output_file = strtok(redir + 2, " ");
                        }
                        else
                        {
                            output_file = strtok(redir + 1, " ");
                        }
                        redirect_output = 1;
                        *redir = '\0';
                        trim_whitespace(commands[i]);
                        trim_whitespace(output_file);
                    }
                }

                pid_t pid = fork();
                if (pid == 0)
                { // Child process
                    if (redirect_input && i == 0)
                    {
                        int fd_input = open(input_file, O_RDONLY);
                        if (fd_input < 0)
                        {
                            perror("No such input file found!");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd_input, STDIN_FILENO);
                        close(fd_input);
                    }
                    else
                    {
                        dup2(in_fd, STDIN_FILENO);
                    }

                    if (redirect_output && i == cmd_count - 1)
                    {
                        int fd_output;
                        if (append)
                        {
                            fd_output = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                        }
                        else
                        {
                            fd_output = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        }
                        if (fd_output < 0)
                        {
                            perror("Error opening output file");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd_output, STDOUT_FILENO);
                        close(fd_output);
                    }
                    else if (i < cmd_count - 1)
                    {
                        dup2(pipe_fds[1], STDOUT_FILENO);
                    }

                    close(pipe_fds[0]);
                    close(pipe_fds[1]);

                    // Execute the command (existing code)
                    if (strncmp(commands[i], "hop ", 4) == 0 || strcmp(commands[i], "hop") == 0)
                    {

                        strcpy(prevwd, hop(commands[i], home, prevwd));
                    }
                    else if (strncmp(commands[i], "reveal ", 7) == 0 || strcmp(commands[i], "reveal") == 0)
                    {
                        // printf("command : %s",commands[i]);
                        exec_list(commands[i], home, prevwd);
                    }
                    else if (strncmp(commands[i], "proclore ", 9) == 0 || strcmp(commands[i], "proclore") == 0)
                    {
                        print_proc(commands[i]);
                    }
                    else if (strncmp(commands[i], "seek ", 5) == 0 || strcmp(commands[i], "seek") == 0)
                    {
                        seek(commands[i]);
                    }
                    else if (strncmp(commands[i], "log ", 4) == 0 || strcmp(commands[i], "log") == 0)
                    {
                        if (strstr(commands[i], "execute"))
                        {
                            char *command = strtok(commands[i], " ");
                            char *subcommand = strtok(NULL, " ");
                            char *index_str = strtok(NULL, " ");
                            if (index_str != NULL)
                            {
                                int index = atoi(index_str);
                                char *command_to_execute = execute(index, log_file);
                                command_to_execute[strlen(command_to_execute) - 1] = '\0';
                                // printf("%s\n",command_to_execute);
                                executecmd(command_to_execute, home, prevwd);
                            }
                        }
                        else
                        {
                            handle_log(commands[i], log_file);
                        }
                    }
                    else if (strncmp(commands[i], "iMan ", 5) == 0)
                    {
                        executeman(commands[i]);
                    }
                    else if (strncmp(commands[i], "activities", 10) == 0)
                    {
                        activities();
                    }
                    else if (strncmp(commands[i], "ping ", 5) == 0)
                    {
                        execping(commands[i]);
                    }
                    else if (strncmp(commands[i], "neonate", 7) == 0)
                    {
                        char *time_arg = strtok(commands[i] + 8, " ");
                        if (time_arg != NULL)
                        {
                            neonate_command(time_arg);
                        }
                        else
                        {
                            printf("Usage: neonate -n [time_arg]\n");
                        }
                    }
                    else if (strncmp(commands[i], "bg ", 3) == 0)
                    {
                        char *bg_temp = strtok(commands[i], " ");
                        char *arg_bg = strtok(NULL, " ");
                        int argb = atoi(arg_bg);
                        ping(argb, 18);
                    }

                    else if (strncmp(commands[i], "fg ", 2) == 0)
                    {
                        char *pid_str = commands[i] + 3; // Skip "fg " to get to the PID
                        if (pid_str && *pid_str)
                        {
                            pid_t pid = atoi(pid_str);
                            fg_command(pid);
                        }
                        else
                        {
                            printf("Usage: fg <pid>\n");
                        }
                    }
                    else
                    {
                        char *args[1024];
                        int arg_idx = 0;
                        char *arg = strtok(commands[i], " ");
                        while (arg != NULL)
                        {
                            args[arg_idx++] = arg;
                            arg = strtok(NULL, " ");
                        }
                        args[arg_idx] = NULL;

                        execvp(args[0], args);
                        perror("execvp failed");
                    }

                    exit(1);
                }
                else if (pid > 0)
                { // Parent process
                    waitpid(pid, NULL, 0);
                    close(pipe_fds[1]);
                    if (in_fd != 0)
                        close(in_fd);
                    in_fd = pipe_fds[0];
                }
                else
                {
                    perror("fork failed");
                    exit(1);
                }
            }
            if (in_fd != 0)
                close(in_fd);
        }
        else
        {

            int c = 0;
            int num = count_char_occurrences(cmd_token2, '&');
            char *saveptr2;
            char *cmd_token3 = strtok_r(cmd_token2, "&", &saveptr2);
            while (cmd_token3 != NULL)
            {
                c++;
                char cmd_token[1024];
                strcpy(cmd_token, cmd_token3);
                if (num >= c && num != 0)
                {
                    strcat(cmd_token, " &");
                }
                trim_whitespace(cmd_token);
                char temp[1024];
                strcpy(temp, cmd_token);
                char result[1024];
                get_part_before_whitespace(temp, result);
                if (!is_command_valid(result))
                {
                    // printf("In there\n");
                    printf("Error: '%s' is not a valid command!!\n", result);
                    cmd_token3 = strtok_r(NULL, "&", &saveptr2);
                    continue;
                }
                // printf("%s\n", cmd);
                // Initialize redirection variables
                char *input_file = NULL;
                char *output_file = NULL;
                int append = 0; // 1 for '>>', 0 for '>'
                int redirect_input = 0, redirect_output = 0;

                // Check for I/O redirection in cmd_token
                char *redir = strchr(cmd_token, '<');
                if (redir)
                {
                    redirect_input = 1;
                    input_file = strtok(redir + 1, "");
                    *redir = '\0'; // Split the command from redirection
                    trim_whitespace(input_file);
                    trim_whitespace(cmd_token);
                    char *redir1 = strchr(input_file, '>');
                    if (redir1)
                    {
                        if (*(redir1 + 1) == '>')
                        {
                            append = 1;
                            output_file = strtok(redir1 + 2, " ");
                        }
                        else
                        {
                            output_file = strtok(redir1 + 1, " ");
                        }
                        redirect_output = 1;
                        *redir1 = '\0'; // Split the command from redirection
                        trim_whitespace(input_file);
                        trim_whitespace(output_file);
                    }
                }
                else
                {
                    redir = strchr(cmd_token, '>');
                    if (redir)
                    {
                        if (*(redir + 1) == '>')
                        {
                            append = 1;
                            output_file = strtok(redir + 2, " ");
                        }
                        else
                        {
                            output_file = strtok(redir + 1, " ");
                        }
                        redirect_output = 1;
                        *redir = '\0'; // Split the command from redirection
                        trim_whitespace(cmd_token);
                        trim_whitespace(output_file);
                    }
                }
                int oldin = dup(STDIN_FILENO);
                if (redirect_input)
                {
                    int fd_input = open(input_file, O_RDONLY);
                    if (fd_input < 0)
                    {
                        perror("No such input file found!");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd_input, STDIN_FILENO);
                    close(fd_input);
                }

                int oldout = dup(STDOUT_FILENO);
                if (redirect_output)
                {
                    int fd_output;
                    if (append)
                    {
                        fd_output = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                    }
                    else
                    {
                        fd_output = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    }
                    if (fd_output < 0)
                    {
                        perror("Error opening output file");
                        exit(EXIT_FAILURE);
                    }

                    dup2(fd_output, STDOUT_FILENO);
                    close(fd_output);
                }

                if (strncmp(cmd_token, "hop ", 4) == 0 || strcmp(cmd_token, "hop") == 0)
                {

                    strcpy(prevwd, hop(cmd_token, home, prevwd));
                }
                else if (strncmp(cmd_token, "reveal ", 7) == 0 || strcmp(cmd_token, "reveal") == 0)
                {
                    // printf("command : %s",cmd_token);
                    exec_list(cmd_token, home, prevwd);
                }
                else if (strncmp(cmd_token, "proclore ", 9) == 0 || strcmp(cmd_token, "proclore") == 0)
                {
                    print_proc(cmd_token);
                }
                else if (strncmp(cmd_token, "seek ", 5) == 0 || strcmp(cmd_token, "seek") == 0)
                {
                    seek(cmd_token);
                }
                else if (strncmp(cmd_token, "log ", 4) == 0 || strcmp(cmd_token, "log") == 0)
                {
                    if (strstr(cmd_token, "execute"))
                    {
                        char *command = strtok(cmd_token, " ");
                        char *subcommand = strtok(NULL, " ");
                        char *index_str = strtok(NULL, " ");
                        if (index_str != NULL)
                        {
                            int index = atoi(index_str);
                            char *command_to_execute = execute(index, log_file);
                            command_to_execute[strlen(command_to_execute) - 1] = '\0';
                            // printf("%s\n",command_to_execute);
                            executecmd(command_to_execute, home, prevwd);
                        }
                    }
                    else
                    {
                        handle_log(cmd_token, log_file);
                    }
                }
                else if (strncmp(cmd_token, "iMan ", 5) == 0)
                {
                    executeman(cmd_token);
                }
                else if (strncmp(cmd_token, "activities", 10) == 0)
                {
                    activities();
                }
                else if (strncmp(cmd_token, "ping ", 5) == 0)
                {
                    execping(cmd_token);
                }
                else if (strncmp(cmd_token, "neonate", 7) == 0)
                {
                    char *time_arg = strtok(cmd_token + 8, " ");
                    if (time_arg != NULL)
                    {
                        neonate_command(time_arg);
                    }
                    else
                    {
                        printf("Usage: neonate -n [time_arg]\n");
                    }
                }
                else if (strncmp(cmd_token, "bg ", 3) == 0)
                {
                    char *bg_temp = strtok(cmd_token, " ");
                    char *arg_bg = strtok(NULL, " ");
                    int argb = atoi(arg_bg);
                    ping(argb, 18);
                }

                else if (strncmp(cmd_token, "fg ", 2) == 0)
                {
                    char *pid_str = cmd_token + 3; // Skip "fg " to get to the PID
                    if (pid_str && *pid_str)
                    {
                        pid_t pid = atoi(pid_str);
                        fg_command(pid);
                    }
                    else
                    {
                        printf("Usage: fg <pid>\n");
                    }
                }
                else
                {

                    // printf("Command: %s\n",cmd_token);
                    int background = 0;
                    if (cmd_token[strlen(cmd_token) - 1] == '&')
                    {
                        background = 1;
                        cmd_token[strlen(cmd_token) - 1] = '\0';
                        trim_whitespace(cmd_token);
                    }

                    pid_t pid = fork();
                    if (pid == 0)
                    {
                        if (redirect_input)
                        {
                            int fd_input = open(input_file, O_RDONLY);
                            if (fd_input < 0)
                            {
                                perror("No such input file found!");
                                exit(EXIT_FAILURE);
                            }
                            dup2(fd_input, STDIN_FILENO);
                            close(fd_input);
                        }

                        // Handle output redirection
                        if (redirect_output)
                        {
                            int fd_output;
                            if (append)
                            {
                                fd_output = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                            }
                            else
                            {
                                fd_output = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            }
                            if (fd_output < 0)
                            {
                                perror("Error opening output file");
                                exit(EXIT_FAILURE);
                            }
                            dup2(fd_output, STDOUT_FILENO);
                            close(fd_output);
                        }

                        if (background)
                        {
                            setsid();
                        }

                        char *args[1024];
                        int arg_idx = 0;
                        char *arg = strtok(cmd_token, " ");
                        while (arg != NULL)
                        {
                            args[arg_idx++] = arg;
                            arg = strtok(NULL, " ");
                        }
                        args[arg_idx] = NULL;
                        execvp(args[0], args);
                        fprintf(stderr, "Error: '%s' is not a valid command!!\n", args[0]);

                        exit(1);
                    }
                    else if (pid > 0)
                    {

                        if (background)
                        {
                            add_process(pid, cmd_token);
                            printf("[%d] %d\n", pid, getpid());
                        }
                        else
                        {
                            waitpid(pid, NULL, 0);
                        }
                    }
                    else
                    {
                        perror("fork");
                    }
                }
                dup2(oldin, 0);
                dup2(oldout, 1);
                close(oldin);
                close(oldout);
                cmd_token3 = strtok_r(NULL, "&", &saveptr2);
            }
        }
        cmd_token2 = strtok_r(NULL, ";", &saveptr);
    }
}
int main()
{
    struct sigaction sa;
    sa.sa_handler = &handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    // Set up signal handlers for Ctrl-C and Ctrl-Z
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);
    signal(SIGCHLD, handle_sigchld);
    char cmd[1024];
    char home[1024];
    char prevwd[1024];
    char log_file[1024];
    int log_flag = 0;
    getcwd(home, sizeof(home));
    strcpy(prevwd, home);

    snprintf(log_file, sizeof(log_file), "%s/%s", home, "log.txt");

    while (1)
    {
        ctrl_c_pressed = 0;
        ctrl_z_pressed = 0;

        prompt(home);

        if (fgets(cmd, sizeof(cmd), stdin) == NULL)
        {
            if (feof(stdin))
            {
                // Ctrl-D was pressed
                printf("\nLogging out...\n");
                break;
            }
            continue;
        }

        cmd[strcspn(cmd, "\n")] = '\0';

        if (ctrl_c_pressed || ctrl_z_pressed)
        {
            continue; // Skip processing this input if Ctrl-C or Ctrl-Z was pressed
        }

        if (strstr(cmd, "log") == NULL)
        {
            logcmd(cmd, log_file);
        }
        if (strcmp(cmd, "exit") == 0)
        {
            break;
        }

        executecmd(cmd, home, prevwd);
    }

    // Kill all background processes before exiting
    // kill_all_processes();

    return 0;
}