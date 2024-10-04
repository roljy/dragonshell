# Dragonshell

Dragonshell is a lightweight shell for Unix-based operating systems.
It was written to directly invoke system calls for process management and
interprocess communication, reducing overhead when interfacing with the OS.


## Design

The design philosophy for dragonshell was to write C code that prioritized
modularity and separation of concerns. Each distinct feature of Dragonshell
is either handled by its own method at its core, or passes parameters to a
method that handles very similar tasks with slight variations.

The source code modularity also extends to the file level: main.c runs the
top-level program from a highly abstracted viewpoint, shellio.c contains
methods dealing with user interface and messaging, internals.c handles core
internal features of the shell, and externals.c handles all the features
dealing with creating and executing child processes.

Through this design philosophy, the lengths and complexities of the methods
were minimized, allowing for more naturally-flowing code.


## Features & System Calls

Each *feature* is listed with the `C method(s)` that was written to
implement it, along with the **system calls** used within those methods.

Built-in commands
* *cd* :
    * `change_dir()`
        * **chdir(2)**
* *pwd* :
    * `print_working_dir()`
        * **getcwd(2)**
* *exit* :
    * `exit_shell()`
        * **kill(2)** to gracefully terminate any background child processes
        * **waitpid(2)** to wait for any such processes to finish terminating
        * **getrusage(2)** to compute the cpu usage times of spawned children

Commands for external programs
* *launch program* :
    * `parse_external_request()`
        * `exec_program()`
            * **fork(2)**
            * `assign_sighandler()`
                * **sigaction(2)** setting the handler to **SIG_DFL**
            * `child_exec_cmd()`
                * **execve(2)**
                * **_exit(2)**
            * `parent_wait_to_close()`
                * **waitpid(2)**
* *background execution* :
    * Same flow as *launch program*, EXCEPT:
        * No call to **waitpid(2)**
* *IO redirection with > and <* :
    * Same flow as *launch program*, EXCEPT:
        * **open(2)** within `parse_external_request()`
        * **dup2(2)** and **close(2)** within `child_exec_cmd()`
        * **close(2)** within `parent_wait_to_close()`
* *pipe 1st cmd output to 2nd cmd input* :
    * Similar flow as *IO redirection*, EXCEPT:
        * **pipe(2)** instead of **open(2)**
        * 2 calls to **fork(2)** in `exec_program()`, one for each command
* *handle C-c and C-z signals* :
    * `assign_sighandler()`
        * **sigaction(2)** setting the handler to **SIG_IGN**


## Testing
