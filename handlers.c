// handlers.c
// Tawfeeq Mannan

#include <stdio.h>      // printf
#include <stdlib.h>     // free
#include <unistd.h>     // chdir, getcwd, _exit, fork, execve
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // wait

#include "constants.h"
#include "shellio.h"
#include "handlers.h"


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
 * @brief Execute an external program as its own process and wait for it
 *        to finish before returning control to shell.
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of strings containing args (start with program filepath)
 */
void exec_external_cmd(int argc, char **argv)
{
    if (argc <= 0)
        perror("Not enough arguments for exec");
    pid_t pid = fork();  // create child
    if (pid == 0)
    {
        // child continues here
        char *envp[1] = { NULL };
        // can pass argv directly since argv[0] is expected to be the filename
        execve(argv[0], argv, envp);
        // execve returning means it failed. assume unknown command
        log_error_msg(EC_UNKNOWN_CMD);
        _exit(1);
    }
    else if (pid > 0)
    {
        // parent continues here
        wait(NULL);  // wait for child to finish
    }
    else
    {
        // pid < 0
        perror("fork() failed");
    }
}
