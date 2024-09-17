// dragonshell.c
// Tawfeeq Mannan

#include <string.h>     // strtok, strcspn
#include <stdio.h>      // printf, fgets
#include <stdlib.h>     // malloc, free

// useful constants
const size_t LINE_LENGTH = 100;  // num of characters per input line
const size_t MAX_ARGS = 5;  // num of args per command
const size_t MAX_LENGTH = 20;  // num of characters per arg
const size_t MAX_BG_PROC = 1; // num of background processes


/**
 * @brief Tokenize a C string
 *
 * @param str C string to tokenize
 * @param delim C string containing delimiter character(s)
 * @param argv char* array that will contain the tokenized strings.
 *             Make sure that you allocate enough space for the array.
 */
void tokenize(char *str, const char *delim, char **argv)
{
    char *token;
    token = strtok(str, delim);
    for (size_t i = 0; token != NULL; ++i)
    {
        argv[i] = token;
        token = strtok(NULL, delim);
    }
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

    // get a command from the user
    char buffer[LINE_LENGTH];
    display_prompt(buffer);

    // split the user-provided command into tokens for parsing
    char **args = malloc(MAX_ARGS * sizeof(char *));
    tokenize(buffer, " ", args);
    printf("You typed: \n");
    for (size_t i = 0; i < 3; i++)
    {
        printf("%s\n", args[i]);
    }
    free(args);

    return 0;
}
