// internals.c
// Tawfeeq Mannan

// C includes
#define _POSIX_SOURCE   // needed for kill() to be declared
#include <string.h>     // strcmp
#include <stdio.h>      // printf
#include <stdlib.h>     // free
#include <unistd.h>     // chdir, getcwd, _exit
#include <signal.h>     // kill, sigaction
#include <sys/wait.h>   // waitpid

// user includes
#include "constants.h"
#include "shellio.h"
#include "internals.h"
#include "externals.h"

// global vars
extern int bg_proc_exists;  // defined in externals.c
extern int bg_child_pid;


/**
 * @brief Assign a function to handle a signal interrupt
 * 
 * @param signum Signal number (eg. SIGINT)
 * @param handler Function pointer to call for this signal, or SIG_IGN, etc.
 */
void assign_sighandler(int signum, void (*handler)(int))
{
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = (*handler);
    sigaction(signum, &sa, NULL);  // don't care about old behaviour
}


/**
 * @brief Central master function to handle all requests,
 *        delegating to subroutines as necessary.
 * 
 * @param argc Number of input arguments (tokens)
 * @param argv Array of strings containing input arguments
 */
void handle_request(int argc, char **argv)
{
    if (argc == 0)  // empty line. no-op
        return;

    if (strcmp(argv[0], "cd") == 0)
    {
        if (argc < 2)
            log_error_msg(EC_CD_NO_ARGS);
        else
            change_dir(argv[1]);  // only needs argv[1]; ignore any after
    }

    else if (strcmp(argv[0], "pwd") == 0)
    {
        print_working_dir();  // no need for any other args
    }

    else if (strcmp(argv[0], "exit") == 0)
    {
        exit_shell();
    }

    else  // assume external command
    {
        parse_external_request(argc, argv);
    }
}


/**
 * @brief Change the current working directory
 * 
 * @param target Absolute or relative path of target dir
 */
void change_dir(const char *target)
{
    int rc = chdir(target);
    if (rc != 0)
        log_error_msg(EC_CD_PATH_NOT_FOUND);
        // no need to do anything if rc == 0; cd was successful
}


/**
 * @brief Print the current working directory
 */
void print_working_dir()
{
    // specifying NULL for buf ensures wd is adequately sized
    char *wd = getcwd(NULL, 0);
    printf("%s\n", wd);
    // since getcwd used malloc to allocate for wd, must free manually
    free(wd);
}


/**
 * @brief Exit the shell gracefully.
 *        All background processes are terminated via SIGTERM.
 */
void exit_shell()
{
    if (bg_proc_exists)
    {
        if (kill(bg_child_pid, SIGTERM) == -1)
            perror("kill() failed (terminating children)");
        // wait for the child to actually terminate before continuing
        else if (waitpid(bg_child_pid, NULL, 0) == -1)
            perror("waitpid() failed (waiting for child to die)");
    }
    // TODO compute child execution times
    _exit(0);
}
