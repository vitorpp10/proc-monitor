#include<iostream>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/types.h>
#include<cstdio>
#include<csignal>
#include<string>

int child_pid_global = -1;
int global_var = 100;

void handlerSignal(int signum)
{
  std::string msg = "\nSIGNAL> Ctrl+C detectado, (Sinal " + std::to_string(signum) + ")\n";

  ssize_t w = write(STDOUT_FILENO, msg.c_str(), msg.size());
  if (w < 0) { exit(EXIT_FAILURE); }

  if (child_pid_global > 0)
  {
    std::string msg1 = "SIGNAL> Encerrando processo filho PID: " + std::to_string(child_pid_global) + "...\n";

    ssize_t w1 = write(STDOUT_FILENO, msg1.c_str(), msg1.size());
    if (w1 < 0) { exit(EXIT_FAILURE); }
    
    kill(child_pid_global, SIGTERM);
  }
}

struct ProcessInfo
{
  pid_t pid;
  long start_time;
  long end_time;
};

int main(int argc, char* argv[])
{
  signal(SIGINT, handlerSignal);

  if (argc < 2)
  {
    std::cout << "Uso correto: " << argv[0] << " <comando> [argumentos...]\n";
    exit(EXIT_FAILURE);
  }

  ProcessInfo* info = new ProcessInfo;
  info->pid = -1;
  info->start_time = 0;
  info->end_time = 0;

  int fildes[2];
  if (pipe(fildes) < 0) { perror("pipe -1"); exit(EXIT_FAILURE); }

  info->pid = fork();
  child_pid_global = info->pid;
  if (info->pid < 0) { perror("fork -1"); exit(EXIT_SUCCESS); }

  if (info->pid == 0)
  {
     global_var = 200;
     std::cout << "global_var alterada para: " << global_var << '\n' << "endereco da global_var: " << &global_var << std::endl;

     close(fildes[0]);

     dup2(fildes[1], STDOUT_FILENO);
     dup2(fildes[1], STDERR_FILENO);

     close(fildes[1]);

     int cmd = execvp(argv[1], &argv[1]);
     if (cmd < 0) 
     { 
       perror("execvp -1"); 
       delete(info); 
       exit(EXIT_FAILURE); 
     }
  }
  else
  {
    close(fildes[1]);
   
    char buffer[1024];
    while (true)
    {
      ssize_t r = read(fildes[0], buffer, sizeof(buffer) - 1);

      if (r > 0)
      {
        buffer[r] = '\0';
        std::cout << buffer;
      }
      else
      {
        break;
      }
    }
    waitpid(info->pid, NULL, 0);

    std::cout << "global var: " << global_var << '\n';
    std::cout << "global var endereco: " << &global_var << '\n';
  }
  delete(info);
  return EXIT_SUCCESS;
}
