// externals.c
// Tawfeeq Mannan

// C includes
#include <string.h>     // strcmp
#include <stdio.h>      // printf
#include <unistd.h>     // fork, execve, close, dup2, pipe
#include <sys/types.h>  // pid_t
#include <signal.h>     // SIGINT, SIGTSTP, SIG_DFL
#include <sys/wait.h>   // waitpid
#include <fcntl.h>      // open

// user includes
#include "constants.h"
#include "shellio.h"
#include "internals.h"
#include "externals.h"

// global vars
int num_bg_proc = 0;
int bg_pids[MAX_BG_PROC * 3];


/**
 * @brief Identify the IO redirects applied to an external command and run it
 * 
 * @param argc Number of input arguments (tokens)
 * @param argv Array of strings containing input arguments
 */
void parse_external_request(int argc, char **argv)
{
    int is_bg_proc = (argc >= 2 && strcmp(argv[argc-1], "&") == 0);
    char *infile, *outfile;
    int infile_fd = STDIN_FILENO, outfile_fd = STDOUT_FILENO;
    int pipe_ends[2];
    char **argv2 = NULL;

    // iterate thru all "possibly sandwiched" args to look for in/out/pipe
    for (int i = 1; i < argc-1; i++)
    {
        if (strcmp(argv[i], "<") == 0)
        {
            // next argument is the input file. open it and assign a fd
            infile = argv[i+1];
            infile_fd = open(infile, O_RDONLY);
            if (infile_fd == -1)
            {
                perror("open() failed (input redirect)");
                infile_fd = STDIN_FILENO;
            }
            argv[i] = NULL;  // cut off the command args for exec_cmd
        }
        else if (strcmp(argv[i], ">") == 0)
        {
            // next argument is the output file. open it and assign a fd
            outfile = argv[i+1];
            outfile_fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (outfile_fd == -1)
            {
                perror("open() failed (output redirect)");
                outfile_fd = STDOUT_FILENO;
            }
            argv[i] = NULL;  // cut off the command args for exec_cmd
        }
        else if (strcmp(argv[i], "|") == 0)
        {
            // everything after this arg is the second command
            argv2 = argv + i + 1;
            if (pipe(pipe_ends) == -1)
            {
                perror("pipe() failed");
            }
            else
            {
                infile_fd = pipe_ends[0];
                outfile_fd = pipe_ends[1];
            }
            argv[i] = NULL;  // cut off the command args for LHS
        }
    }

    if (is_bg_proc)
    {
        // need to set the final "&" to a NULL if it exists
        argv[--argc] = NULL;
    }

    // can pass same args regardless of pipe or not,
    // exec_program will decide how to handle fds accordingly based on argv2
    exec_program(argv, argv2, is_bg_proc, infile_fd, outfile_fd);
}


/**
 * @brief Execute an external program (or 2) as its own process.
 * 
 * @param argv Array of strings containing args (start with program filepath)
 * @param argv Array of strings containing second command args
 * @param is_bg_proc True if process should run in background, False otherwise
 * @param input_fd File descriptor of input file
 * @param output_fd File descriptor of output file
 */
void exec_program(char **argv,
                  char **argv2,
                  int is_bg_proc,
                  int input_fd,
                  int output_fd)
{
    int is_pipe_case = (argv2 != NULL);
    pid_t cpid1, cpid2;

    cpid1 = fork();  // create child
    if (cpid1 == 0)
    {
        // child continues here
        if (is_pipe_case)
        {
            if (input_fd != STDIN_FILENO && close(input_fd) == -1)
                perror("close() failed");
            input_fd = STDIN_FILENO;
        }
        child_exec_cmd(argv, is_bg_proc, input_fd, output_fd);
        // child_exec_cmd() never returns so child is done now
    }
    else if (cpid1 < 0)
    {
        perror("fork() failed");
        return;
    }

    // parent continues here.
    // should create another child for RHS if piping, otherwise skip to waiting
    if (is_pipe_case)
    {
        cpid2 = fork();
        if (cpid2 == 0)
        {
            // child2 continues here for RHS cmd
            if (output_fd != STDOUT_FILENO && close(output_fd) == -1)
                perror("close() failed");
            output_fd = STDOUT_FILENO;
            child_exec_cmd(argv2, is_bg_proc, input_fd, output_fd);
            // child_exec_cmd() never returns so child2 is done now
        }
        else if (cpid1 < 0)
        {
            perror("fork() failed");
            return;
        }

        // parent continues here in the pipe case
        // need to split close since we can't re-close the same fd twice
        parent_wait_to_close(cpid1, is_bg_proc, STDIN_FILENO, output_fd);
        parent_wait_to_close(cpid2, is_bg_proc, input_fd, STDOUT_FILENO);
    }

    else  // parent continues here in non-pipe case
    {
        parent_wait_to_close(cpid1, is_bg_proc, input_fd, output_fd);
    }
}


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
                    int output_fd)
{
    char *envp[1] = { NULL };
    assign_sighandler(SIGINT, SIG_DFL);
    assign_sighandler(SIGTSTP, SIG_DFL);

    // redirect input to come from input file. no-op if infile == stdin
    if (dup2(input_fd, STDIN_FILENO) == -1)
        perror("dup2() failed (input redirect)");
    else if (input_fd != STDIN_FILENO)
        close(input_fd);  // stdin points to the input file now

    // redirect output to output file. no-op if outfile == stdout
    if (dup2(output_fd, STDOUT_FILENO) == -1)
        perror("dup2() failed (output redirect)");
    else if (output_fd != STDOUT_FILENO)
        close(output_fd);  // stdout points to the output file now

    // ! REMOVED: No longer need to redirect output to /dev/null

    // argv[0] will be ignored when passing args so can pass argv directly
    execve(argv[0], argv, envp);
    // execve returning means it failed. assume unknown command
    log_error_msg(EC_UNKNOWN_CMD);
    _exit(1);
}


/**
 * @brief Wait for a given child to finish (if applicable), then
 *        clean up resources when the child has terminated.
 * 
 * @param pid Child's process ID
 * @param is_bg_proc True if parent should let child run in bg,
 *                   False if parent should wait for child to finish in fg
 * @param input_fd File descriptor of input file
 * @param output_fd File descriptor of output file
 */
void parent_wait_to_close(pid_t pid,
                          int is_bg_proc,
                          int input_fd,
                          int output_fd)
{
    if (is_bg_proc)
    {
        // TODO capture SIGCHLD to remove zombies from list
        bg_pids[num_bg_proc++] = pid;
        printf("PID %d is sent to background\n", pid);
    }

    else
    {
        // wait for child to finish. use waitpid instead of wait in case a
        // bg process coincidentally finishes before the fg process
        if (waitpid(pid, NULL, WUNTRACED) == -1)
            perror("waitpid() failed");

        // since child is done now, we can close in/out files if necessary
        if (input_fd != STDIN_FILENO && close(input_fd) == -1)
            perror("close() failed (input file)");
        if (output_fd != STDOUT_FILENO && close(output_fd) == -1)
            perror("close() failed (output file)");
    }
}
