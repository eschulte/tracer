#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include "libelf.h"

int main(int argc, char *argv[]){
  FILE * fd = stdout;
  int status, begin, end, c;
  pid_t child;
  struct user_regs_struct regs;
  long ins;

  /* parse command line options */
  while ((c = getopt(argc, argv, "ho:")) != -1)
    switch(c)
      {
      case 'h':
        fprintf(stderr,
                "Usage: tracer [-o FILE] PROGRAM [ARG...]\n"
                "Run PROGRAM on ARGs printing each value of the program\n"
                "counter to FILE or to STDOUT if FILE is not specified.\n");
        return 1;
      case 'o':
        fd = fopen(argv[2], "w");
        break;
      default:
        abort();
      }

  /* ensure the file exists */
  if(access(argv[optind], F_OK) == -1){
    fprintf(stderr,"program file `%s' does not exist.\n", argv[optind]);
    return 1;
  }

  /* get on with it */
  begin = get_text_address(argv[optind]);
  end   = begin + get_text_offset(argv[optind]);
  switch (child=fork()){
  case -1: // error
    printf("fork error\n");
    return 1;
    break;
  case 0:  // child
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    // don't let child print to STDOUT if we're writing to STDOUT
    if (fd == stdout)
      freopen("/dev/null", "a", stdout);
    execvp(argv[optind], &argv[optind]);
    break;
  default: // parent
    while(1) {
      wait(&status);
      if(WIFEXITED(status)) break;
      ptrace(PTRACE_GETREGS, child, NULL, &regs);
      // ins = ptrace(PTRACE_PEEKTEXT, child, regs.rip, NULL);
      // fprintf(fd, "0x%lx 0x%lx\n", regs.rip, ins);
      if ((begin <= regs.PC_REG) && (end >= regs.PC_REG))
        fprintf(fd, "%d\n", regs.rip);
      ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
    }
    return status;
    break;
  }
}
