// dragonshell.c
// Tawfeeq Mannan

// C includes
#include <string.h>     // memset
#include <stdio.h>      // printf

// user includes
#include "constants.h"
#include "shellio.h"
#include "handlers.h"


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
