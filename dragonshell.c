// dragonshell.c
// Tawfeeq Mannan

#include <string.h>


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


int main(int argc, char *argv[])
{
    return 0;
}
