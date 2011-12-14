#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#define STDOUT 1
#define STDERR 2

void main(int argc, char *argv[]) {
  int  wait_val;                    // child's return value
  int fd = open("/dev/null", "w+"); // for redirecting STDIN and STDOUT
  pid_t  pid;                       // child's process id
  struct user_regs_struct regs;     // for user_regs_struct

  switch (pid=fork()) {
  case -1: perror("fork"); break;
  case 0: // child
    // redirect STDOUT and STDERR to /dev/null
    if(dup2(fd, STDOUT) != 0) printf("dup stdout failed.");
    if(dup2(fd, STDERR) != 0) printf("dup stderr failed.");
    ptrace(0,0,0,0);
    // execute the child process
    execvp(argv[1], &argv[1]);
    break;
  default: // parent
    wait(&wait_val);
    if (ptrace(PTRACE_SINGLESTEP,pid,0,0) != 0)          perror("ptrace");
    wait(&wait_val);
    while (wait_val == 1407) {
      if (ptrace(PTRACE_SINGLESTEP,pid,0,0) != 0)        perror("ptrace");
      if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) != 0) perror("ptrace");
      printf("0x%lx is 0x%lx\n", ptrace(PTRACE_PEEKTEXT, pid, regs.rip, NULL));
      wait(&wait_val);
    }
  }
}
