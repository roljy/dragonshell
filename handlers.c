// handlers.c
// Tawfeeq Mannan

// C includes
#define _POSIX_SOURCE   // needed for kill() to be declared
#include <string.h>     // strcmp
#include <stdio.h>      // printf
#include <stdlib.h>     // free
#include <unistd.h>     // chdir, getcwd, _exit, fork, execve
#include <signal.h>     // kill
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid
#include <fcntl.h>      // open

// user includes
#include "constants.h"
#include "shellio.h"
#include "handlers.h"

// global vars
int bg_proc_exists = 0;
int child_pid;


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
        // TODO kill children
    }
    _exit(0);
}


/**
 * @brief Execute an external program as its own process.
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of strings containing args (start with program filepath)
 * @param is_bg_proc True if process should run in background, False otherwise
 * @param infile Filepath of input file, or NULL for stdin
 * @param outfile Filepath of output file, or NULL for stdout
 */
void exec_external_cmd(int argc,
                       char **argv,
                       int is_bg_proc,
                       char *infile,
                       char *outfile)
{
    if (argc <= 0)
        perror("Not enough arguments for exec");

    pid_t pid = fork();  // create child
    if (pid == 0)
    {
        // child continues here
        char *envp[1] = { NULL };
        // TODO redirect bg process output to devnull using dup2
        // argv[0] will be ignored when passing args so can pass argv directly
        execve(argv[0], argv, envp);
        // execve returning means it failed. assume unknown command
        log_error_msg(EC_UNKNOWN_CMD);
        _exit(1);
    }
    else if (pid > 0)
    {
        // parent continues here.
        if (is_bg_proc)
        {
            // TODO make a list of children, to support >1 bg process
            // would need to capture SIGCHLD and remove child from list
            bg_proc_exists = 1;
            child_pid = pid;
            printf("PID %d is sent to background\n", child_pid);
        }
        else
        {
            // wait for child to finish. use waitpid instead of wait in case a
            // bg process coincidentally finishes before the fg process
            waitpid(pid, NULL, 0);
        }
    }
    else
    {
        // pid < 0
        perror("fork() failed");
    }
}
