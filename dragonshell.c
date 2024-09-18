// dragonshell.c
// Tawfeeq Mannan

// C includes
#include <string.h>     // strcmp
#include <stdio.h>      // printf
#include <stdbool.h>    // true/false

// user includes
#include "constants.h"
#include "shellio.h"
#include "handlers.h"

// global vars
bool keep_shell_alive = true;


/**
 * @brief Central master function to handle all requests,
 *        delegating to subroutines as necessary.
 * @param argc Number of input arguments (tokens)
 * @param argv char* array containing input arguments
 */
void handle_request(int argc, char **argv)
{
    if (argc == 0)  // empty line. no-op
        return;

    if (strcmp(argv[0], "cd") == 0)
    {
        if (argc < 2)
            log_error_msg(EC_CD_NO_ARGS);
        else
            change_dir(argv[1]);  // only needs argv[1]; ignore any after
    }

    else if (strcmp(argv[0], "pwd") == 0)
    {
        print_working_dir();  // no need for any other args
    }

    else if (strcmp(argv[0], "exit") == 0)
    {
        // TODO gracefully exit
        keep_shell_alive = false;
    }

    else  // assume external command
    {
        exec_external_cmd(argc, argv);
    }
}


/**
 * @brief Main function. Program entry point.
 * 
 * @param argc Command-line argument count
 * @param argv char* array of command-line arguments
 * 
 * @return Exit status (0 on success)
 */
int main(int argc, char *argv[])
{
    // display welcome message at start
    printf("Welcome to Dragon Shell!\n\n");

    while (keep_shell_alive)
    {
        // get a command from the user
        char buffer[LINE_LENGTH];
        display_prompt(buffer);

        // split the user-provided command into tokens for parsing
        char *args[MAX_ARGS * 3] = { NULL };  // x3 for safety
        size_t args_cnt = tokenize(buffer, " ", args);

        // handle the arguments accordingly
        handle_request(args_cnt, args);
    }

    return 0;
}
