# mini-project-1-template
# C Shell

## Overview

This custom shell is designed to support a variety of functionalities including command execution, piping, redirection, background and foreground process management, and more. The shell allows you to execute built-in commands as well as external commands with various options for redirection and background execution.

## Features

- **Command Execution**: Execute external commands and built-in commands.
- **Piping**: Support for piping between commands using `|`.
- **Redirection**: Input and output redirection with `<`, `>`, and `>>`.
- **Background Jobs**: Execute commands in the background using `&`.
- **Foreground Jobs**: Bring background jobs to the foreground using `fg`.
- **Process Management**: List and manage processes.
- **Custom Commands**: Includes custom commands like `hop`, `reveal`, `proclore`, `seek`, `log`, `iMan`, `activities`, `ping`, and `neonate`.

## Commands

### Built-in Commands

- **`hop`**: Changes the working directory. Usage: `hop <path>`
- **`reveal`**: Displays information about the system or environment. Usage: `reveal <options>`
- **`proclore`**: Lists current processes. Usage: `proclore`
- **`seek`**: Searches for a pattern in files or directories. Usage: `seek <pattern>`
- **`log`**: Handles log files. Usage: `log <options>`
- **`iMan`**: Displays information about the manual. Usage: `iMan <options>`
- **`activities`**: Lists recent activities. Usage: `activities`
- **`ping`**: Pings a specified target. Usage: `ping <target>`
- **`neonate`**: Creates a new neonate process. Usage: `neonate -n <time>`

### External Commands

You can execute any external command using the shell. External commands are executed using the `execvp` system call.

## Usage

1. **Starting the Shell**: Compile the shell and run the executable.
2. **Executing Commands**: Type commands directly into the shell prompt.
3. **Piping Commands**: Use `|` to pipe output from one command to another.
4. **Redirecting Output**: Use `>` to redirect output to a file and `>>` to append to a file.
5. **Redirecting Input**: Use `<` to redirect input from a file.
6. **Background Execution**: Append `&` to a command to run it in the background.
7. **Foreground Execution**: Use `fg <pid>` to bring a background process to the foreground.
# Prompt Function

### File: `prompt.h`

### Function: `prompt`

- **Purpose**: Displays a shell prompt with the username, hostname, and current working directory, using color formatting.

- **Parameters**: 
  - `char* home`: User’s home directory for abbreviation.

- **Logic**:
  1. **Retrieve User/Host Info**: 
     - Uses `getpwuid()` for username and `gethostname()` for hostname.
  2. **Handle CWD**: 
     - Retrieves the current directory using `getcwd()`.
     - If inside `home`, abbreviates path with `~`, else shows the full path.
  3. **Display Prompt**:
     - Uses ANSI color codes to style the username (blue), hostname (green), and directory (cyan).
  4. **Error Handling**: 
     - Prints errors if fetching username, hostname, or CWD fails.
  
- **Color Reset**: After each segment, terminal colors are reset using the `RESET` macro.


# Hop Function

### File: `hop.h`

### Function: `hop`

- **Purpose**: Changes the current working directory based on input arguments.

- **Parameters**: 
  - `char* argum`: The command input, e.g., `hop` or a directory path.
  - `char* home`: User’s home directory for `~` expansion.
  - `char* pevcwd`: Previous working directory, used for `-` option.

- **Logic**:
  1. **Default to Home**:
     - If `argum` is `"hop"`, changes to the home directory and prints the path.
  2. **Process Arguments**:
     - Splits `argum` using `strtok_r()`.
     - Handles special cases:
       - `"-"`: Switches to the previous directory.
       - `"~"`: Switches to the home directory.
       - Otherwise, changes to the provided path.
  3. **Update Previous Directory**:
     - Stores the current working directory in `pevcwd` before changing to the new path.
  4. **Error Handling**:
     - Prints an error if directory change (`chdir()`) fails.
     - Prints the current directory on successful change.

- **Return**: Returns the updated previous working directory (`pevcwd`).

# Reveal Functions

### File: `reveal.h`

### Functions:

#### `list`
- **Purpose**: Lists files in a directory with optional hidden file filtering.
- **Parameters**:
  - `int hidden_flag`: If set, includes hidden files.
  - `const char* dir_path`: Path of the directory to list.
  - `char* home`: User’s home directory for `~` expansion.
  - `char* prevcd`: Previous working directory for `-` option.
- **Logic**:
  - Resolves `-` and `~` to proper paths.
  - Filters hidden files based on `hidden_flag`.
  - Prints directory contents with color-coding: Blue (directories), Green (executables), White (regular files).

#### `print_file_info`
- **Purpose**: Prints detailed file info (permissions, owner, size, etc.).
- **Parameters**:
  - `const char* name`: File name.
  - `const struct stat* sb`: File status info (metadata).

#### `list_all`
- **Purpose**: Lists all files with detailed info, using `print_file_info()`.
- **Parameters**: Same as `list`.

#### `exec_list`
- **Purpose**: Parses and executes the list command, supporting flags like `-a` (show hidden) and `-l` (long format).
- **Parameters**:
  - `char* cmd`: The full command input (e.g., `ls -al`).
  - `char* home`, `char* prevcd`: Same as `list`.

---

# Log Functions

### File: `log.h`

### Functions:

#### `logcmd`
- **Purpose**: Logs executed commands, avoiding duplicates.
- **Parameters**:
  - `char* cmd`: Command to log.
  - `const char* log_file`: Log file path.

#### `writecmd`
- **Purpose**: Writes a command to the log file, maintaining a fixed log size.
- **Parameters**: Same as `logcmd`.

#### `purge`
- **Purpose**: Clears the log file.
- **Parameters**:
  - `const char* log_file`: Log file path.

#### `handle_log`
- **Purpose**: Manages log commands like viewing and purging.
- **Parameters**:
  - `char* cmd_token`: The log command.
  - `const char* log_file`: Log file path.

#### `execute`
- **Purpose**: Retrieves and executes a command from the log by number.
- **Parameters**:
  - `int number`: The command index.
  - `const char* log_file`: Log file path.
- **Returns**: The command string at the specified log index.

# Proclore Functions

### File: `proclore.h`

### Functions:

#### `get_process_info`
- **Purpose**: Retrieves and displays information about a process by its PID.
- **Parameters**:
  - `pid_t pid`: The process ID to query.
- **Logic**:
  1. **Process Status**: Reads from `/proc/[pid]/status` to extract:
     - Process state.
     - Process group ID (`Tgid`).
     - Virtual memory size (`VmSize`).
  2. **Executable Path**: Reads the symbolic link from `/proc/[pid]/exe` to display the path of the executable.
  3. **Error Handling**: Prints an error if the process does not exist or if it fails to read the executable path.

#### `print_proc`
- **Purpose**: Parses a command to print process information for a given or current PID.
- **Parameters**:
  - `char* cmd`: Command input that optionally includes a PID.
- **Logic**:
  - Extracts the PID from the command, defaults to the current process ID if no PID is provided.
  - Calls `get_process_info()` to display process details.

# Code Explanation

## Seek Functions

### `colorprint`
- **Purpose**: Prints the file or directory name with a color based on its type (directory or file).
- **Parameters**:
  - `char *path`: Path or name of the file/directory.
  - `int is_dir`: Flag to indicate if the path is a directory (prints blue) or a file (prints green).

### `check_file`
- **Purpose**: Compares a file name with a target file, accounting for possible extensions.
- **Parameters**:
  - `char *a`: The base name to check.
  - `char *target`: The target file name.
- **Logic**: 
  - If the base name contains an extension, it checks for an exact match.
  - If the base name doesn't have an extension, it checks if the base name matches the beginning of the target name with a `.` added.

### `recurse`
- **Purpose**: Recursively searches through directories to find files or directories matching the target name.
- **Parameters**:
  - `char *base_path`: The starting directory path.
  - `char *target`: The name of the file or directory to search for.
  - `int search_files`: Whether to search for files.
  - `int search_dirs`: Whether to search for directories.
  - `int *found_count`: Tracks the number of matches found.
  - `char *invoke`: The current working directory.
  - `char *found_path`: Stores the first found path.

### `seek`
- **Purpose**: Parses the input command and initiates a search for a file or directory within the target directory.
- **Parameters**:
  - `char *command_string`: The command input that includes options (`-d`, `-f`, `-e`), target file or directory, and search directory.
- **Flags**:
  - `-d`: Search for directories only.
  - `-f`: Search for files only.
  - `-e`: Execute the found file or change directory to the found directory.

---

## Process Management (activities.h)

### `add_process`
- **Purpose**: Adds a process to the process management system.
- **Parameters**:
  - `pid_t pid`: The process ID of the process.
  - `const char *command`: The command associated with the process.

### `update_process_status`
- **Purpose**: Checks the status of all tracked processes, updating whether they are running, stopped, or terminated.
- **Logic**:
  - Uses `waitpid` to check for process status changes and updates the internal process list accordingly.
  - Handles processes that have exited or have been stopped/continued.

### `compare_processes`
- **Purpose**: Helper function for sorting processes based on their command names.
- **Parameters**:
  - Two `Process` structures to be compared.

### `fg_command`
- **Purpose**: Brings a process to the foreground and resumes it if it was stopped.
- **Parameters**:
  - `pid_t pid`: The process ID to bring to the foreground.
- **Logic**:
  - Sets the terminal's foreground process group to the specified process.
  - Sends the `SIGCONT` signal to resume the process if it was stopped.

### `activities`
- **Purpose**: Displays a list of all running and stopped processes, sorted by command name.
- **Logic**:
  - Calls `update_process_status` to ensure process statuses are up-to-date.
  - Sorts the process list using `qsort` and `compare_processes`.
  - Displays each process, its PID, command, and whether it's stopped or running.

---
# Code Explanation

## Ping Functions

### `ping`
- **Purpose**: Sends a signal to a process.
- **Parameters**:
  - `int pid`: The process ID to which the signal will be sent.
  - `int sig`: The signal number to send (e.g., `SIGKILL`, `SIGTERM`).
- **Logic**:
  - Uses the `kill` system call to send a signal to the process.
  - If the process doesn't exist or an error occurs, it prints an error message.
  - Otherwise, it prints a success message indicating that the signal was sent.

### `execping`
- **Purpose**: Parses a command string and sends a signal to a process.
- **Parameters**:
  - `char *cmd`: The command string, which includes the process ID and signal.
- **Logic**:
  - The command string is split using `strtok` to extract the PID and signal number.
  - The `ping` function is called with these extracted values.

---

## Neonate Functions

### `handle_signal`
- **Purpose**: Handles signals sent to the process.
- **Parameters**:
  - `int sig`: The signal number (e.g., `SIGUSR1`).
- **Logic**:
  - If the signal is `SIGUSR1`, it sets the `running` flag to 0, which can be used to terminate the loop in the child process.

### `get_most_recent_pid`
- **Purpose**: Retrieves the most recent PID from the system.
- **Logic**:
  - Reads the `/proc/sys/kernel/pid_max` file to get the most recent PID.
  - If successful, returns the PID. Otherwise, returns -1.

### `getch`
- **Purpose**: Captures a single character input from the user without echoing it to the screen.
- **Logic**:
  - Uses `tcgetattr` and `tcsetattr` to modify terminal settings, disabling canonical mode and echo.
  - Returns the captured character.

### `neonate_command`
- **Purpose**: Continuously prints the most recent process ID at specified intervals until the user presses the 'x' key.
- **Parameters**:
  - `const char *time_arg`: The time interval (in seconds) between prints.
- **Logic**:
  - Parses the time argument, ensuring it's a valid positive integer.
  - Uses `fork` to create a child process:
    - **Child Process**: 
      - Continuously fetches and prints the most recent PID every `interval` seconds until it receives a `SIGUSR1` signal.
    - **Parent Process**:
      - Waits for user input. If the 'x' key is pressed, the parent sends a `SIGUSR1` signal to the child process, terminating it.
  - If the `fork` fails, prints an error message.

---

# IMAN Function

## `fetch_data_from_url`

### Purpose
Sends an HTTP GET request to a given URL and prints the text content of the response while ignoring HTML tags, scripts, and styles.

### Parameters
- **`const char *hostname`**: The hostname of the server to connect to.
- **`const char *path`**: The path of the resource on the server.

### Functionality
1. **Prepare Address Info**:
   - Initializes `hints` for `getaddrinfo` to handle both IPv4 and IPv6 addresses and TCP stream sockets.

2. **Get Address Information**:
   - Calls `getaddrinfo` to resolve the hostname into an address structure.

3. **Create Socket**:
   - Creates a socket using the resolved address information.

4. **Connect to Server**:
   - Connects the socket to the server's address.

5. **Prepare and Send HTTP GET Request**:
   - Constructs an HTTP GET request string.
   - Sends the request using the socket.

6. **Receive and Process Response**:
   - Receives the response in chunks.
   - Parses the response to extract and print text content, ignoring HTML tags, scripts, and styles.

7. **Close Socket**:
   - Closes the socket after receiving the response.

### Error Handling
- Prints error messages if `getaddrinfo`, `socket`, `connect`, or `send` fail.
- Prints an error message if receiving the response fails.

## `executeman`

### Purpose
Constructs a URL path based on a command argument and fetches data from the URL using the `fetch_data_from_url` function.

### Parameters
- **`char* cmd`**: The command string containing the argument for the URL path.

### Functionality
1. **Parse Command String**:
   - Splits the command string into tokens using `strtok`.
   - Extracts the argument for the URL path.

2. **Construct URL Path**:
   - Uses `snprintf` to create the URL path with the given argument.

3. **Fetch Data**:
   - Calls `fetch_data_from_url` with the hostname (`man.he.net`) and the constructed path.

### Example Usage
If `cmd` is `"manpage topic"`, it will construct the path `/?topic=topic&section=all` and fetch data from `man.he.net` using this path.

---

# Shell Functions and Signal Handlers

## Signal Handlers

### `handle_sigint`

**Purpose**: Handles the SIGINT signal (usually generated by pressing Ctrl+C).

**Functionality**:
- Sets the global flag `ctrl_c_pressed`.
- Sends SIGINT to the foreground process if it exists.
- Prints a new line if no foreground process is running.

### `handle_sigtstp`

**Purpose**: Handles the SIGTSTP signal (usually generated by pressing Ctrl+Z).

**Functionality**:
- Sets the global flag `ctrl_z_pressed`.
- Sends SIGTSTP to the foreground process if it exists.

### `handle_sigchld`

**Purpose**: Handles the SIGCHLD signal, which is sent to a process when a child process terminates or stops.

**Functionality**:
- Reaps all terminated child processes.
- Prints status messages based on the termination status:
  - **Exit Status**: Prints when a background process exits normally.
  - **Signal Termination**: Prints when a background process is terminated by a signal.
  - **Stopped**: Prints when a process is stopped; adds the process to the background processes list.

## Utility Functions

### `is_command_valid`

**Purpose**: Checks if a command is valid by verifying if it is a known command or if it exists in the system's PATH.

**Parameters**:
- **`const char *command`**: The command to check.

**Returns**:
- `1` if the command is valid.
- `0` otherwise.

### `get_part_before_whitespace`

**Purpose**: Extracts the part of a string before the first whitespace.

**Parameters**:
- **`const char *str`**: The input string.
- **`char *result`**: Buffer to store the result.

**Functionality**:
- Copies the substring before the first whitespace into the result buffer.
- If no whitespace is found, copies the entire string.

### `trim_whitespace`

**Purpose**: Trims leading and trailing whitespace from a string.

**Parameters**:
- **`char *str`**: The string to trim.

**Functionality**:
- Removes leading and trailing spaces from the string.
- Modifies the original string in place.

### `count_char_occurrences`

**Purpose**: Counts occurrences of a specific character in a string.

**Parameters**:
- **`const char *str`**: The input string.
- **`char ch`**: The character to count.

**Returns**:
- The number of occurrences of the specified character.

## Notes

- **Global Variables**:
  - `volatile sig_atomic_t ctrl_c_pressed`: Flag set when Ctrl+C is pressed.
  - `volatile sig_atomic_t ctrl_z_pressed`: Flag set when Ctrl+Z is pressed.
  - `pid_t foreground_pid`: Stores the PID of the foreground process.

- **Signal Handling**:
  - Ensure proper handling of child processes and background tasks by implementing appropriate functions to manage process states and interactions.
