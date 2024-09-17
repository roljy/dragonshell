// dragonshell.c
// Tawfeeq Mannan

#include <string.h>     // strtok, strcspn, strcmp
#include <stdio.h>      // printf, fgets
#include <stdbool.h>    // true/false
#include <unistd.h>     // chdir

// useful constants
const size_t LINE_LENGTH = 100;  // num of characters per input line
const size_t MAX_ARGS = 5;  // num of args per command
const size_t MAX_LENGTH = 20;  // num of characters per arg
const size_t MAX_BG_PROC = 1; // num of background processes

// global vars
bool keep_shell_alive = true;


/**
 * @brief Tokenize a C string
 *
 * @param str C string to tokenize
 * @param delim C string containing delimiter character(s)
 * @param argv char* array that will contain the tokenized strings.
 *             Make sure that you allocate enough space for the array.
 * 
 * @return Number of split-up tokens
 */
size_t tokenize(char *str, const char *delim, char **argv)
{
    char *token;
    token = strtok(str, delim);
    size_t i;
    for (i = 0; token != NULL; ++i)
    {
        argv[i] = token;
        token = strtok(NULL, delim);
    }
    return i;
}


/**
 * @brief Display the prompt and take a line of input
 * 
 * @param buffer C string to store the user input. Space must be pre-allocated.
 */
void display_prompt(char *buffer)
{
    printf("dragonshell > ");
    fgets(buffer, LINE_LENGTH, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}


/**
 * @brief Log an error message
 * 
 * @param msg Explanation of the error that occurred
 */
void log_error(char *msg)
{
    printf("dragonshell: %s\n", msg);
}


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
        {
            log_error("Expected argument to \"cd\"");
            return;
        }
        // cd only needs argv[1], can ignore anything that follows
        if (chdir(argv[1]) != 0)
        {
            log_error("No such file or directory");
        }
    }

    else if (strcmp(argv[0], "exit") == 0)
    {
        // TODO gracefully exit
        keep_shell_alive = false;
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
        char *args[MAX_ARGS * 3];  // x3 for safety, in case of multi-command
        size_t args_cnt = tokenize(buffer, " ", args);

        // handle the arguments accordingly
        handle_request(args_cnt, args);
    }

    return 0;
}
