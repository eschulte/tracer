#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <sys/ptrace.h>
#include <sys/user.h>
// adapted from
// http://www.comp.glam.ac.uk/pages/staff/hggross/IntelligentTimingAnalysis_HTML/node20.html
#define STDOUT 1
#define STDERR 2

void main(int argc, char *argv[]) {
  int  wait_val;         // child's return value
  int  pid;              // child's process id
  int fd = open("/dev/null", "w+");

  struct user_regs_struct regs;

  switch (pid=fork()) {
  case -1: perror("fork"); break;
  case 0: // child
    // redirect STDOUT and STDERR to /dev/null
    if(dup2(fd, STDOUT) != 0) printf("dup stdout failed.");
    if(dup2(fd, STDERR) != 0) printf("dup stderr failed.");
    ptrace(0,0,0,0);
    // must be called in order to allow the control over the child process
    execvp(argv[1], &argv[1]);
    // executes the testprogram and causes the child to stop and send
    // a signal to the parent, the parent can now switch to PTRACE_SINGLESTEP
    break;
    // child process ends
  default: // parent
    wait(&wait_val);
    // parent waits for child to stop (execl)
    if (ptrace(PTRACE_SINGLESTEP,pid,0,0) != 0)          perror("ptrace");
    // switch to singlestep tracing and release child
    wait(&wait_val);
    // wait for child to stop at next instruction
    while (wait_val == 1407) {
      if (ptrace(PTRACE_SINGLESTEP,pid,0,0) != 0)        perror("ptrace");
      if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) != 0) perror("ptrace");
      printf("%x\n", regs.rip);
      wait(&wait_val);
    }
  }
}
