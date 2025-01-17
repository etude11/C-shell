#include "iman.h"

// Function to create and send HTTP GET request, and read the response
void fetch_data_from_url(const char *hostname, const char *path) {
    int sockfd;
    struct addrinfo hints, *res;
    char request[1024], buffer[BUFFER_SIZE];
    int bytes_received;

    // Prepare hints for getaddrinfo
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // Either IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    // Get address info of the hostname
    if (getaddrinfo(hostname, PORT, &hints, &res) != 0) {
        fprintf(stderr, "Error in getaddrinfo\n");
        return;
    }

    // Create a socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("Error creating socket");
        freeaddrinfo(res);
        return;
    }

    // Connect to the server
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("Error connecting to server");
        close(sockfd);
        freeaddrinfo(res);
        return;
    }

    // Free the address info structure
    freeaddrinfo(res);

    // Prepare the HTTP GET request
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             path, hostname);

    // Send the HTTP request to the server
    if (send(sockfd, request, strlen(request), 0) == -1) {
        perror("Error sending request");
        close(sockfd);
        return;
    }

    // Variables for parsing HTML
    int in_tag = 0;
    int in_script = 0;
    int in_style = 0;

    // Receive the response and print text content
    while ((bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the buffer
        
        for (char *p = buffer; *p; p++) {
            if (*p == '<') {
                in_tag = 1;
                if (strncmp(p, "<script", 7) == 0) in_script = 1;
                if (strncmp(p, "<style", 6) == 0) in_style = 1;
            } else if (*p == '>') {
                in_tag = 0;
                if (strncmp(p - 8, "</script>", 9) == 0) in_script = 0;
                if (strncmp(p - 7, "</style>", 8) == 0) in_style = 0;
            } else if (!in_tag && !in_script && !in_style) {
                if (isprint(*p) || *p == '\n') {
                    putchar(*p);
                }
            }
        }
    }

    if (bytes_received == -1) {
        perror("Error receiving response");
    }

    // Close the socket
    close(sockfd);
}

void executeman(char* cmd) {


    char* cmd1 = strtok(cmd," ");
    char* arg = strtok(NULL," ");
    const char *hostname = "man.he.net";
    char path[MAX_PATH_LENGTH];

    // Construct the path using snprintf
    snprintf(path, sizeof(path), "/?topic=%s&section=all", arg);

    // Fetch data from the URL
    fetch_data_from_url(hostname, path);
}