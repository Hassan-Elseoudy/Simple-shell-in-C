## Unix - Shell

This is a simple shell implemented using C++.

##### Commands

+ The internal shell command "exit" which terminates the shell
    - **Concepts**: shell commands, exiting the shell.

+ A command with no arguments
  - **Example**: ls, cp, rm ...etc
  - Shell must block until the command completes.
  - **Concepts**: Forking a child process, waiting for it to complete and synchronous execution.

+ A command with arguments
  - **Example**: ls –l
  - **Details**: Argument 0 is the name of the command.
  - **Concepts**: Command-line parameters.
  
+ A command, with or without arguments, executed in the background using &.
  - **Example**: firefox &
  - **Details**: In this case, Shell executes the command and return immediately, not blocking until the command finishes.
  - **Concepts**: Background execution, signals, signal handlers, processes and asynchronous execution.


![alt text](https://image.ibb.co/jAYa0q/image.png "Sample Image")
