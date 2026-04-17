// INCLUDE LIBRARIES
#include <stdio.h> // printf, ...
#include <stdbool.h> // bool, ...
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS, exit(), malloc(), free(), ...
#include <string.h> // strings stuff

#include <unistd.h> // close()
#include <sys/socket.h>
#include <netinet/in.h>

#include <dirent.h> // opendir()


// UTILS
typedef char* string;
void validate_cli_args(int argc, string argv[]);
void validate_retrieved_args(int http_port, string log_file, string root_folder);
bool test_existence_directory(string folder);

// MAIN
int main(int argc, string argv[]) {

    // 1. Validate usage is ./server <HTTP PORT> <LOG FILE> <ROOT FOLDER>
    validate_cli_args(argc, argv);
    int http_port = atoi(argv[1]);
    string log_file = argv[2];
    string root_folder = argv[3];
    validate_retrieved_args(http_port, log_file, root_folder);

    printf("[Success] returned 0\n");
    return EXIT_SUCCESS;
}
// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------
// FUNCTIONS

void validate_cli_args(int argc, string argv[]) {
    // Usage: ./server <HTTP PORT> <LOG FILE> <ROOT FOLDER>
    //           (1)       (2)        (3)          (4)
    if (argc != 4) {
        printf("[Error] (too many/few args?) Usage: ./server <HTTP PORT> <LOG FILE> <ROOT FOLDER>\n");
        exit(EXIT_FAILURE);
    }
}

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

    // Check log_file contains the suffixes either ".txt" or ".log"
    int len_log_file = strlen(log_file);
    if (len_log_file <= 4) {
        // Does there exist x s.t x.txt or x.log?
        printf("[Error] <LOG FILE> isn't .txt or .log, or filename is empty\n");
        errors_flag = true;
    } else {
        // How to achieve this? start storing the suffixes
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

    // Check if root_folder exists
    if (test_existence_directory(root_folder)) {
        // Pass
    } else {
        printf("[Error] <ROOT FOLDER> doesn't exist or isn't a directory, or you may not have perms to open it\n");
        errors_flag = true;
    }

    // Exit if any errors
    if (errors_flag) {
        printf("[Error] Need help? Usage is ./server <HTTP PORT> <LOG FILE> <ROOT FOLDER>\n");
        exit(EXIT_FAILURE);
    }
}

bool test_existence_directory(string folder) {
    DIR* dir = opendir(folder);
    if (dir) {
        closedir(dir);
        return true;
    } else {
        return false;
    }
}