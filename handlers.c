// handlers.c
// Tawfeeq Mannan

// C includes
#define _POSIX_SOURCE   // needed for kill() to be declared
#include <string.h>     // strcmp
#include <stdio.h>      // printf
#include <stdlib.h>     // free
#include <unistd.h>     // chdir, getcwd, _exit, fork, execve, close,
                        //  dup2, pipe
#include <signal.h>     // kill, sigaction
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid
#include <fcntl.h>      // open

// user includes
#include "constants.h"
#include "shellio.h"
#include "handlers.h"

// global vars
int bg_proc_exists = 0;
int bg_child_pid;


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
    char **argv2 = argv;

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

    if (argv2 == argv)
    {
        // no pipe. use infile and outfile for the same command
        exec_program(argc, argv, is_bg_proc, infile_fd, outfile_fd);
    }
    else
    {
        // piping output of cmd1 to cmd2. can hardcode is_bg_proc = false
        // can run in series since kernel will buffer pipe contents
        // TODO run these in parallel so cmd2 receives input in real time
        exec_program(argc, argv, 0, STDIN_FILENO, outfile_fd);
        exec_program(argc, argv2, 0, infile_fd, STDOUT_FILENO);
    }
}


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
                  int output_fd)
{
    if (argc <= 0)
        perror("Not enough arguments for exec");

    pid_t pid = fork();  // create child
    if (pid == 0)
    {
        // child continues here
        child_exec_cmd(argv, is_bg_proc, input_fd, output_fd);
    }
    else if (pid > 0)
    {
        // parent continues here
        parent_cleanup_after_exec(pid, is_bg_proc, input_fd, output_fd);
    }
    else
    {
        // pid < 0
        perror("fork() failed");
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

    // redirect bg process output to devnull so it gets hidden
    if (is_bg_proc)
    {
        int devnull_fd = open("/dev/null", O_WRONLY);
        if (devnull_fd == -1)
            perror("open() failed (opening /dev/null)");
        // only redirect if it would have otherwise printed to stdout
        // no need to redirect if it's already going to a file
        if (output_fd == STDOUT_FILENO)
        {
            if (dup2(devnull_fd, STDOUT_FILENO) == -1)
                perror("dup2() failed (output redirect)");
            else if (devnull_fd != STDOUT_FILENO)
                close(devnull_fd);  // stdout points to /dev/null now
        }
    }

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
void parent_cleanup_after_exec(pid_t pid,
                               int is_bg_proc,
                               int input_fd,
                               int output_fd)
{
    if (is_bg_proc)
    {
        // TODO make a list of children, to support >1 bg process
        // would need to capture SIGCHLD and remove child from list
        bg_proc_exists = 1;
        bg_child_pid = pid;
        printf("PID %d is sent to background\n", bg_child_pid);
    }

    else
    {
        // wait for child to finish. use waitpid instead of wait in case a
        // bg process coincidentally finishes before the fg process
        waitpid(pid, NULL, 0);

        // since child is done now, we can close in/out files if necessary
        if (input_fd != STDIN_FILENO)
        {
            if (close(input_fd) == -1)
                perror("close() failed (input file)");
        }
        if (output_fd != STDOUT_FILENO)
        {
            if (close(output_fd) == -1)
                perror("close() failed (output file)");
        }
    }
}
