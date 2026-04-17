// FORMAT SPECIFIERS
// %c -> char
// %d -> signed integer (d = decimal)
// %u -> unsigned integer
// %f -> floating-point numbers
// %s -> string specifiers
// %p -> address specifiers

// TESTING:
// ./server 8080 /home/user/Documents/tws/logger.log /home/user/Desktop/stuff

// MORE TYPES
typedef char* string;
typedef unsigned char* bytes;

// INCLUDE HEADERS
#include <stdio.h> // printf, ...
#include <stdbool.h> // bool, ...
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS, exit(), malloc(), free(), ...
#include <string.h> // strings stuff
#include <dirent.h> // opendir()

#include <sys/types.h> // Common types
#include <sys/socket.h> // socket utils
#include <unistd.h> // unix api, for close(), etc.
#include <netinet/in.h> // protocol utils

// UTILS
const int MAX_CONNECTIONS = 100;

void validate_cli_args(int argc, string argv[]);
void validate_retrieved_args(int http_port, string log_file, string root_folder);
bool test_existence_directory(string folder);
bool is_substring(string flag, string str);
bool starts_with_slash(string path);

void create_server(int http_port);

// MAIN
int main(int argc, string argv[]) {

    // 1. Validate usage is ./server <HTTP PORT> <LOG FILE> <ABS ROOT FOLDER>
    validate_cli_args(argc, argv);
    int http_port = atoi(argv[1]);
    string log_file = argv[2];
    string root_folder = argv[3];
    validate_retrieved_args(http_port, log_file, root_folder);
    if (is_substring(root_folder, log_file)) {
        printf("[Error] Writing logs in <ABS ROOT FOLDER> is not allowed because of race condition errors\n");
        exit(EXIT_FAILURE);
    }
    printf("[Beware] Logs are being written in: %s\n", log_file);
    printf("[Beware] Retrieving resources from: %s\n", root_folder);

    // 2. Create server using the given http_port
    printf("[Beware] Check your IP on the bash\n\n");
    create_server(http_port);

    printf("[Message] Success, returned 0\n");
    exit(EXIT_SUCCESS);
}
// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

// PARSING
//------------------------------------------------------------------------------------------------------
// FUNCTION 1
void validate_cli_args(int argc, string argv[]) {
    // Usage: ./server <HTTP PORT> <LOG FILE> <ABS ROOT FOLDER>
    //           (1)       (2)        (3)          (4)
    if (argc != 4) {
        printf("[Error] (too many/few args?) Usage: ./server <HTTP PORT> <LOG FILE> <ABS ROOT FOLDER>\n");
        exit(EXIT_FAILURE);
    }
}
//------------------------------------------------------------------------------------------------------
// FUNCTION 2
void validate_retrieved_args(int http_port, string log_file, string root_folder) {
    bool errors_flag = false;

    // Check http_port is greater than 0
    if (http_port <= 0) {
        printf("[Error] <HTTP PORT> must be an integer greater than 0\n");
        errors_flag = true;
    }
    // Check log_file is not an integer
    if (atoi(log_file) != 0) {
        printf("[Error] <LOG FILE> should NOT be an integer\n");
        errors_flag = true;
    }

    // Check log_file contains the suffixes either ".txt" or ".log" ==> How to achieve this? (check ELSE block)
    int len_log_file = strlen(log_file);
    if (len_log_file <= 4) {
        // Does there exist x s.t x.txt or x.log?
        printf("[Error] <LOG FILE> isn't .txt or .log, or filename is empty\n");
        errors_flag = true;
    } else {
        // Start by storing the possible suffixes for a logger
        int length_suffixes = 4;
        string suffix1 = ".txt";
        string suffix2 = ".log";
        
        // Initialize file_extension string (on the stack, so entires are mutable)
        // 4 + 1 because of null-terminator '\0'
        char file_extension[4+1] = {0};

        // E.g., string x = "abcdef.txt", then lower = pos(.)
        int lower = len_log_file - 4;
        
        // Usage: strncpy(string, src string ~ index to copy, max of chars to copy)
        strncpy(file_extension, &log_file[lower], length_suffixes);
        file_extension[4] = '\0';

        // Usage: strncmp == 0 iff str1 stringly equals to str2
        if (strcmp(file_extension, suffix1) == 0 || strcmp(file_extension, suffix2) == 0) {
            // Pass
        } else {
            printf("[Error] <LOG FILE> isn't .txt or .log\n");
            errors_flag = true;
        }
    }

    // LOG FILE requires absolute path, and it must be writtable
    if (!starts_with_slash(log_file)) {
        printf("[Error] <LOG FILE> must be given with an absolute path (if it doesn't exist, it will be created in the given directory)\n");
        errors_flag = true;
    } else {
        FILE* pF = fopen(log_file, "a");
        if (pF == NULL) {
            /*
            ~ THIS CODE WAS REMOVED FOR SIMPLICITY ~
            printf("[Error] Does <LOG FILE> target path exists? Do you have perms? ");
            fflush(stdout);
            perror("File pointer says");
            */
            printf("[Error] Does <LOG FILE> target path exists? Do you have perms? Did you end path with /<filename>.txt or /<filename>.log?\n");
            errors_flag = true;
        } else {
            fclose(pF);
        }
    }

    // Check if root_folder exists
    if (!starts_with_slash(root_folder)) {
        printf("[Error] <ROOT FOLDER> must be given with an absolute path and MUST exist already\n");
        errors_flag = true;
    } else if (!test_existence_directory(root_folder)) {
        printf("[Error] <ROOT FOLDER> doesn't exist or isn't a directory, or you may not have perms to open it\n");
        errors_flag = true;
    }

    /*
    ~ THIS CODE WAS REMOVED FOR SIMPLICITY ~
    if (test_existence_directory(root_folder)) {
        if (!starts_with_slash(root_folder)) {
            printf("[Error] <ROOT FOLDER> exists relatively to current path, but you must provide absolute path\n");
            errors_flag = true;
        }
    } else {
        printf("[Error] <ROOT FOLDER> doesn't exist or isn't a directory, or you may not have perms to open it\n");
        errors_flag = true;
    }
    */

    // Exit if any errors
    if (errors_flag) {
        printf("[Error] Need help? Usage is ./server <HTTP PORT> <LOG FILE> <ABS ROOT FOLDER>\n");
        exit(EXIT_FAILURE);
    }
}
//------------------------------------------------------------------------------------------------------
// FUNCTION 3
bool test_existence_directory(string folder) {
    DIR* dir = opendir(folder);
    if (dir) {
        closedir(dir);
        return true;
    } else {
        return false;
    }
}



// SOCKETS (btw, fd <-> file descriptor. Everything is treated like files, even sockets)
//         fd = a structure with an id, e.g., 0 -> file.txt, 1 -> hello.html, 2 -> socket !!!
// -----------------------------------------------------------
// FUNCTION 1
void create_server(int http_port) {
    // int socketfd (domain, type, protocol)
    //      There used to be multiple STREAM types of sockets. However, nowadays TCP
    //      is prolly the most (if not only) used, hence protocol = 0 <-> TCP
    
    //      server_socketfd returns descriptor of -1 if it fails to be created
    // 1. Creating the socket
    int server_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socketfd < 0) {
        printf("[Error] Socket failed to create. ");
        fflush(stdout);
        perror("server_socketfd says");
        exit(EXIT_FAILURE);
    }

    //      sin = (s)ocket (in)ternet? btw host() -> network bytes, INADDR_ANY -> '0.0.0.0'
    // 2. Fetching the address family ~ (HOST, PORT) (order is inverted in C unlike Python)
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(http_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 3. Bind the socket to the specified port, same logic, -1 = failed.
    if (bind(server_socketfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        printf("[Error] Socket failed to bind. ");
        fflush(stdout);
        perror("server_socketfd says");
        exit(EXIT_FAILURE);
    }

    // 4. Listen for incoming clients
    if (listen(server_socketfd, MAX_CONNECTIONS) < 0) {
        printf("[Error] Socket failed to listen. ");
        fflush(stdout);
        perror("server_socketfd says");
        exit(EXIT_FAILURE);
    }
    printf("[Server] Listening on port %d\n", http_port);

    // 5. Handle connections (no threads for now though)
    while (true) {
        int client_socketfd = accept(server_socketfd, NULL, NULL);

        char response[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 22\r\n"
            "\r\n"
            "<h1>Hello world</h1>";

        send(client_socketfd, response, sizeof(response) - 1, 0);

        close(client_socketfd);
    }
}


// I/O, PATH VALIDATION
// ------------------------------------------------------
// FUNCTION 1
bool starts_with_slash(string path) {
    return path[0] == '/';
}
// FUNCTION 2
bool is_substring(string flag, string str) {
    return strstr(str, flag) != NULL;
}
// FUNCTION 3
void logger(string log_file) {
    FILE* pF = fopen(log_file, "a");
    fprintf(pF, "hey");
    fclose(pF);
}