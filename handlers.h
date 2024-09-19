// handlers.h
// Tawfeeq Mannan

#ifndef _HANDLERS_H
#define _HANDLERS_H


/**
 * @brief Central master function to handle all requests,
 *        delegating to subroutines as necessary.
 * @param argc Number of input arguments (tokens)
 * @param argv char* array containing input arguments
 */
void handle_request(int argc, char **argv);


/**
 * @brief Change the current working directory
 * 
 * @param target Absolute or relative path of target dir
 */
void change_dir(const char *target);


/**
 * @brief Print the current working directory
 */
void print_working_dir();


/**
 * @brief Exit the shell gracefully
 */
void exit_shell();


/**
 * @brief Execute an external program as its own process and wait for it
 *        to finish before returning control to shell.
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of strings containing args (start with program filepath)
 */
void exec_external_cmd(int argc, char **argv);


#endif  // _HANDLERS_H
