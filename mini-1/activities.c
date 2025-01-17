#include "activities.h"
static Process processes[MAX_PROCESSES];
static int process_count = 0;

void add_process(pid_t pid, const char *command) {
    if (process_count < MAX_PROCESSES) {
        processes[process_count].pid = pid;
        strncpy(processes[process_count].command, command, sizeof(processes[process_count].command) - 1);
        processes[process_count].stopped = 0;
        process_count++;
    }
}

void update_process_status() {
    int i = 0;
    while (i < process_count) {
        int status;
        pid_t result = waitpid(processes[i].pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
        if (result > 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                
                for (int j = i; j < process_count - 1; j++) {
                    processes[j] = processes[j + 1];
                }
                process_count--;
                continue; 
            } else if (WIFSTOPPED(status)) {
                processes[i].stopped = 1; 
            } else if (WIFCONTINUED(status)) {
                processes[i].stopped = 0; 
            }
        } else if (result == 0) {
            
            if (kill(processes[i].pid, 0) == -1 && errno == ESRCH) {
                
                for (int j = i; j < process_count - 1; j++) {
                    processes[j] = processes[j + 1];
                }
                process_count--;
                continue; 
            }
            
            processes[i].stopped = 0;
        } else {
            
            for (int j = i; j < process_count - 1; j++) {
                processes[j] = processes[j + 1];
            }
            process_count--;
            continue; 
        }
        i++;
    }
}

static int compare_processes(const void *a, const void *b) {
    return strcmp(((Process *)a)->command, ((Process *)b)->command);
}

void fg_command(pid_t pid) {
    int process_found = 0;
    int process_index = -1;

    // Find the process in our list
    for (int i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            process_found = 1;
            process_index = i;
            break;
        }
    }

    if (!process_found) {
        printf("No such process found.\n");
        return;
    }

    // Bring the process to foreground
    tcsetpgrp(STDIN_FILENO, pid);

    // Send SIGCONT signal to the process
    if (kill(pid, SIGCONT) < 0) {
        perror("kill (SIGCONT)");
        return;
    }

    int status;
    waitpid(pid, &status, WUNTRACED);

    // Check if the process was stopped
    if (WIFSTOPPED(status)) {
        printf("Process %d stopped.\n", pid);
        processes[process_index].stopped = 1;
    } else {
        // Process has finished, remove it from our list
        for (int i = process_index; i < process_count - 1; i++) {
            processes[i] = processes[i + 1];
        }
        process_count--;
    }

    // Give control back to the shell
    tcsetpgrp(STDIN_FILENO, getpgrp());
}
void activities() {
    update_process_status();
    
    qsort(processes, process_count, sizeof(Process), compare_processes);

    for (int i = 0; i < process_count; i++) {
        printf("%d : %s - %s\n", 
               processes[i].pid, 
               processes[i].command, 
               processes[i].stopped ? "Stopped" : "Running");
    }
}
