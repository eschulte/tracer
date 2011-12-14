#include <stdlib.h>
#include <signal.h>
#include <syscall.h>
#include <sys/ptrace.h>

void main (void) {

  long long counter = 1; // machine instruction counter
  int  wait_val;         // child's return value
  int  pid;              // child's process id

  switch (pid=fork()) {
  case -1: perror("fork"); break;
  case 0:                     // child process starts
    ptrace(0,0,0,0);
    // must be called in order to allow the
    // control over the child process
    execl("./hello","hello",NULL);
    // executes the testprogram and causes
    // the child to stop and send a signal 
    // to the parent, the parent can now
    // switch to PTRACE_SINGLESTEP
    break;
    // child process ends
  default:                    // parent process starts
    wait(&wait_val);
    // parent waits for child to stop (execl)
    if (ptrace(PTRACE_SINGLESTEP,pid,0,0) != 0)
      perror("ptrace");
    // switch to singlestep tracing and release child
    wait(&wait_val);
    // wait for child to stop at next instruction
    while (wait_val == 1407) {
      counter++;
      if (ptrace(PTRACE_SINGLESTEP,pid,0,0) != 0) 
        perror("ptrace");
      wait(&wait_val);
    }
    // continue to stop, wait and release until
    // the child is finished; wait_val != 1407
    // Low=0177L and High=05 (SIGTRAP)
  }
  printf("Counter: %d\n", counter);
}
