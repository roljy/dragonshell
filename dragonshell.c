// dragonshell.c
// Tawfeeq Mannan

// C includes
#include <string.h>     // strcmp, memset
#include <stdio.h>      // printf
#include <stdbool.h>    // true/false

// user includes
#include "constants.h"
#include "shellio.h"
#include "handlers.h"


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
        exit_shell();
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
    char buffer[LINE_LENGTH];
    char *tokens[MAX_ARGS * 3];  // x3 for safety
    size_t token_cnt;

    // display welcome message at start
    printf("Welcome to Dragon Shell!\n\n");

    while (1)
    {
        // wipe the buffer and arguments
        memset(buffer, 0, sizeof(buffer));
        memset(tokens, 0, sizeof(tokens));

        // get a command from the user
        display_prompt(buffer);

        // split the user-provided command into tokens for parsing
        token_cnt = tokenize(buffer, " ", tokens);

        // handle the arguments accordingly
        handle_request(token_cnt, tokens);
    }

    return 1;  // should never be here, exit is handled by exit_shell()
}
