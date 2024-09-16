// dragonshell.c
// Tawfeeq Mannan

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// useful constants
const size_t LINE_LENGTH = 100;  // # characters per input line
const size_t MAX_ARGS = 5;  // # args per command
const size_t MAX_LENGTH = 20;  // # characters per arg
const size_t MAX_BG_PROC = 1; // # background processes


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


void display_prompt(char **argv)
{
    printf("dragonshell > ");

    char *buffer;
    buffer = (char *) malloc(LINE_LENGTH * sizeof(char));
    getline(&buffer, (size_t*)&LINE_LENGTH, stdin);
    tokenize(buffer, " ", argv);
    free(buffer);
}


int main(int argc, char *argv[])
{
    // welcome message at shell start
    printf("Welcome to Dragon Shell!\n\n");

    char **args;
    // TODO allocate array of strings correctly
    args = (char **) malloc(MAX_ARGS * MAX_LENGTH * sizeof(char));
    display_prompt(args);
    printf("You typed: \n");
    for (size_t i = 0; i < MAX_ARGS; i++)
        printf("%s\n", args[i]);
    free(args);

    return 0;
}
