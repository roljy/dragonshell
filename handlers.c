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
#include <sys/wait.h>   // wait

// user includes
#include "constants.h"
#include "shellio.h"
#include "handlers.h"

// global vars
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
 *        All background processes must be terminated via SIGTERM.
 */
void exit_shell()
{
    // TODO implement using kill and _exit syscalls
    exit(0);
}


/**
 * @brief Execute an external program as its own process.
 *        If argv ends with "&", send it to background,
 *        otherwise wait for it to finish before returning control to shell.
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of strings containing args (start with program filepath)
 */
void exec_external_cmd(int argc, char **argv)
{
    if (argc <= 0)
        perror("Not enough arguments for exec");

    int is_bg_proc = (argc >= 2 && strcmp(argv[argc-1], "&") == 0);

    pid_t pid = fork();  // create child
    if (pid == 0)
    {
        // child continues here
        char *envp[1] = { NULL };
        // argv[0] will be ignored when passing args so can pass argv directly,
        // but may need to set the final "&" to a NULL if it exists
        if (is_bg_proc)
        {
            argv[argc-1] = NULL;
            // TODO redirect output to devnull using dup2
        }
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
