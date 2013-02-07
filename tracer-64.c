#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include "libelf.h"

int main(int argc, char *argv[]){
  FILE * fd;
  int status, begin, end;
  pid_t child;
  struct user_regs_struct regs;
  long ins;

  /* check for help flag */
  if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'h'){
    printf("Usage: tracer [program] [args...]\n"
           "Run PROGRAM on ARGS recording the memory address of each executed\n"
           "assembly instruction to the file 'trace'.\n");
    return 0; }

  /* get on with it */
  fd    = fopen("trace", "a");
  begin = get_text_address(argv[1]);
  end   = begin + get_text_offset(argv[1]);
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
      // ins = ptrace(PTRACE_PEEKTEXT, child, regs.rip, NULL);
      // fprintf(fd, "0x%lx 0x%lx\n", regs.rip, ins);
      if ((begin <= regs.rip) && (end >= regs.rip))
        fprintf(fd, "%d\n", regs.rip);
      ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
    }
    return status;
    break;
  }
}
