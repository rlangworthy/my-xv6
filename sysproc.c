#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "syscall.h"

char *syscalllist[NSYSCALLS+1]= {
[0]           ="total syscalls",
[SYS_fork]    ="fork",
[SYS_exit]    ="exit",
[SYS_wait]    ="wait",
[SYS_pipe]    ="pipe",
[SYS_read]    ="read",
[SYS_kill]    ="kill",
[SYS_exec]    ="exec",
[SYS_fstat]   ="fstat",
[SYS_chdir]   ="chdir",
[SYS_dup]     ="dup",
[SYS_getpid]  ="getpid",
[SYS_sbrk]    ="sbrk",
[SYS_sleep]   ="sleep",
[SYS_uptime]  ="uptime",
[SYS_open]    ="open",
[SYS_write]   ="write",
[SYS_mknod]   ="mknod",
[SYS_unlink]  ="unlink",
[SYS_link]    ="link",
[SYS_mkdir]   ="mkdir",
[SYS_close]   ="close",
[SYS_countTraps] ="countTraps",
};

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int 
sys_countTraps(void)
{
  struct proc *p =myproc();
  int i;
  int total = 0;
  for(i = 0;i<NSYSCALLS;i++){
    cprintf("%d %d\n", i+1, p->traps[i]);
    total += p->traps[i];
  }
  cprintf("Total number of traps for process %d: %d\n", p->pid, total);
  return sizeof(int)*NSYSCALLS;
  
}
