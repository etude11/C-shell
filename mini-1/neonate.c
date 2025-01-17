#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include "neonate.h"

// Ensure only one definition of the variable
volatile sig_atomic_t running = 1;

// Signal handler for keyboard input
void handle_signal(int sig) {
    if (sig == SIGUSR1) {
        running = 0;
    }
}

// Function to get the most recent PID
pid_t get_most_recent_pid() {
    FILE *fp;
    char path[1024];
    pid_t pid = -1;
    snprintf(path, sizeof(path), "/proc/sys/kernel/pid_max");

    fp = fopen(path, "r");
    if (fp) {
        fscanf(fp, "%d", &pid);
        fclose(fp);
    }
    return pid;
}

// Function to get a single character from stdin
char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void neonate_command(const char *time_arg) {
    int interval = atoi(time_arg);
    if (interval <= 0) {
        fprintf(stderr, "Invalid time argument. Please provide a positive integer.\n");
        return;
    }

    // Set up signal handler for keyboard input
    struct sigaction sa;
    sa.sa_handler = &handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);

    // Set up signal handling for key 'x'
    pid_t child_pid = fork();
    if (child_pid == 0) {
        // Child process
        while (running) {
            pid_t recent_pid = get_most_recent_pid();
            if (recent_pid != -1) {
                printf("%d\n", recent_pid);
                fflush(stdout);
            }
            sleep(interval);
        }
        exit(0);
    } else if (child_pid > 0) {
        // Parent process
        while (1) {
            char ch = getch();
            if (ch == 'x') {
                kill(child_pid, SIGUSR1); // Send signal to terminate child process
                wait(NULL); // Wait for child process to terminate
                break;
            }
        }
    } else {
        perror("fork");
    }
}
