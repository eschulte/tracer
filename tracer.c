#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#define STDOUT 1
#define STDERR 2

int main(int argc, char *argv[]){
  int fd = open("/dev/null", "w+");
  int status;
  long ins;
  pid_t child;
  struct user_regs_struct regs;
  child = fork();
  
  switch (child=fork()){
  case -1:
    printf("fork error\n");
    return 1;
    break;
  case 0:  // child
    if(dup2(fd, STDOUT) != 0) printf("dup stdout failed.\n");
    if(dup2(fd, STDERR) != 0) printf("dup stderr failed.\n");
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execvp(argv[1], &argv[1]);
    break;
  default: // parent
    while(1) {
      wait(&status);
      if(WIFEXITED(status)) break;
      ptrace(PTRACE_GETREGS, child, NULL, &regs);
      // ins = ptrace(PTRACE_PEEKTEXT, child, regs.eip, NULL);
      // printf("0x%lx 0x%lx\n", regs.eip, ins);
      printf("0x%lx\n", regs.eip);
      ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
    }
    return status;
    break;
  }
}
