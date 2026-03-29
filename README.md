# proc-monitor

this project is a low-level system utility written in c++ that acts as a wrapper for executing terminal commands. it demonstrates core concepts of process management, inter-process communication (ipc), and linux memory isolation.

## features

* **process creation & execution:** uses `fork()` to create a child process and `execvp()` to run a user-specified command passed via terminal arguments.
* **i/o redirection:** utilizes `pipe()` and `dup2()` to capture the child's standard output (`stdout`) and standard error (`stderr`), redirecting them back to the parent process which reads and prints the buffer.
* **graceful termination:** implements a custom `sigint` (ctrl+c) handler. if the program is interrupted, the parent intercepts the signal and safely terminates the running child process using `kill()` with `sigterm` before cleaning up.
* **memory isolation proof:** intentionally modifies a global variable (`global_var`) in both the parent and child processes to demonstrate copy-on-write (cow). it proves that despite sharing the exact same virtual memory address, the os isolates their physical memory state.

## core concepts explored

* process lifecycle (`fork`, `execvp`, `waitpid`)
* inter-process communication (`pipe`, `read`, `write`)
* file descriptor manipulation (`dup2`, `close`, `stdout_fileno`)
* posix signals (`signal`, `kill`, `sigint`, `sigterm`)
* virtual vs physical memory abstraction

## how to run

compile the program:
```
g++ -wall -wextra -o3 project.cpp -o monitor
```

run:
```
./monitor ls -la
./monitor ping google.com
```
