// externals.h
// Tawfeeq Mannan

#ifndef _EXTERNALS_H
#define _EXTERNALS_H

#include <sys/types.h>      // pid_t


/**
 * @brief Identify the IO redirects applied to an external command and run it
 * 
 * @param argc Number of input arguments (tokens)
 * @param argv Array of strings containing input arguments
 */
void parse_external_request(int argc, char **argv);


/**
 * @brief Execute an external program as its own process.
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of strings containing args (start with program filepath)
 * @param is_bg_proc True if process should run in background, False otherwise
 * @param input_fd File descriptor of input file
 * @param output_fd File descriptor of output file
 */
void exec_program(int argc,
                  char **argv,
                  int is_bg_proc,
                  int input_fd,
                  int output_fd);


/**
 * @brief Launch a program in a child process.
 * 
 * ! WARNING: Because it invokes execve(), this function never returns, and the
 * ! caller process will DIE after calling this, regardless of success/failure.
 * 
 * @param argv Null-terminated array of strings containing command & all args
 * @param is_bg_proc True if process should run in background, False otherwise
 * @param input_fd File descriptor of input file
 * @param output_fd File descriptor of output file
 */
void child_exec_cmd(char **argv,
                    int is_bg_proc,
                    int input_fd,
                    int output_fd);


/**
 * @brief Wait for a given child to finish (if applicable), then
 *        clean up resources when the child has terminated.
 * 
 * @param pid Child's process ID
 * @param is_bg_proc True if child should run in background, False otherwise
 * @param input_fd File descriptor of input file
 * @param output_fd File descriptor of output file
 */
void parent_cleanup_after_exec(pid_t pid,
                               int is_bg_proc,
                               int input_fd,
                               int output_fd);


#endif  // _EXTERNALS_H