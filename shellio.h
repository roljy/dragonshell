// shellio.h
// Tawfeeq Mannan

#ifndef _SHELLIO_H
#define _SHELLIO_H

#include <stddef.h>  // size_t

#include "constants.h"


/**
 * @brief Print the prompt and take a line of user input
 * 
 * @param buffer char array to store user input. Space must be pre-allocated.
 */
void display_prompt(char *buffer);


/**
 * @brief Tokenize a C string
 *
 * @param str C string to tokenize
 * @param delim C string containing delimiter character(s)
 * @param argv Array of strings that will contain the tokenized strings.
 *             Ensure that enough space is allocated.
 * 
 * @return Number of split-up tokens
 */
size_t tokenize(char *str, const char *delim, char **argv);


/**
 * @brief Print a descriptive error message given a corresponding code
 * 
 * @param rc Code for the error raised
 */
void log_error_msg(ErrCode rc);


#endif  // _SHELLIO_H
