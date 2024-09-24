// handlers.h
// Tawfeeq Mannan

#ifndef _HANDLERS_H
#define _HANDLERS_H

#include <sys/types.h>      // pid_t


/**
 * @brief Assign a function to handle a signal interrupt
 * 
 * @param signum Signal number (eg. SIGINT)
 * @param handler Function pointer to call for this signal, or SIG_IGN, etc.
 */
void assign_sighandler(int signum, void (*handler)(int));


/**
 * @brief Central master function to handle all requests,
 *        delegating to subroutines as necessary.
 * 
 * @param argc Number of input arguments (tokens)
 * @param argv Array of strings containing input arguments
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


#endif  // _HANDLERS_H
