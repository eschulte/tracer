#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include "libelf.h"

int main(int argc, char *argv[]){
  FILE * fd = fopen("trace", "a");
  int status;
  pid_t child;
  struct user_regs_struct regs;
  int begin = get_text_address(argv[1]);
  int end   = begin + get_text_offset(argv[1]);
  long ins;
  child = fork();

  switch (child=fork()){
  case -1: // error
    printf("fork error\n");
    return 1;
    break;
  case 0:  // child
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execvp(argv[1], &argv[1]);
    break;
  default: // parent
    while(1) {
      wait(&status);
      if(WIFEXITED(status)) break;
      ptrace(PTRACE_GETREGS, child, NULL, &regs);
      // ins = ptrace(PTRACE_PEEKTEXT, child, regs.eip, NULL);
      // fprintf(fd, "0x%lx 0x%lx\n", regs.eip, ins);
      if ((begin <= regs.eip) && (end >= regs.eip))
        fprintf(fd, "%d\n", regs.eip);
      ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
    }
    return status;
    break;
  }
}
