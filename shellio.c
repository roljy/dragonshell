// shellio.c
// Tawfeeq Mannan

#include <string.h>     // strcspn, strtok
#include <stdio.h>      // printf, fgets

#include "constants.h"
#include "shellio.h"


/**
 * @brief Print the prompt; take a line of user input from stdin
 * 
 * @param buffer char array to store the resulting input.
 *               Space must be pre-allocated for fgets().
 */
void display_prompt(char *buffer)
{
    printf("dragonshell > ");
    fgets(buffer, LINE_LENGTH, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}


/**
 * @brief Tokenize a C string
 *
 * @param str C string to tokenize
 *            (cannot be const because strtok() modifies in place)
 * @param delim C string containing delimiter character(s)
 * @param argv Array of strings that will contain the tokenized strings.
 *             Ensure that enough space is allocated.
 * 
 * @return Number of split-up tokens (which can be used as a future argc)
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
 * @brief Print a descriptive error message given a corresponding code
 * 
 * @param rc Code for the error raised
 */
void log_error_msg(ErrCode rc)
{
    switch (rc)
    {
    case EC_SUCCESS:
        return;
    case EC_CD_NO_ARGS:
        printf("dragonshell: Expected argument to \"cd\"\n");
        break;
    case EC_CD_PATH_NOT_FOUND:
        printf("dragonshell: No such file or directory\n");
        break;
    case EC_UNKNOWN_CMD:
        printf("dragonshell: Command not found\n");
        break;
    default:
        printf("dragonshell: Unknown error code!\n");
        printf("Ensure all errors have been added to enum ErrCode.\n");
    }
}
