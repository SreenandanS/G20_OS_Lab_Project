#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_forkprio(void)
{
  int priority;

  argint(0, &priority);
  return kforkprio(priority);
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_sigalarm(void)
{
  int ticks;
  uint64 handler;

  argint(0, &ticks);
  argaddr(1, &handler);
  return sigalarm(ticks, handler);
}

uint64
sys_sigreturn(void)
{
  return sigreturn();
}

uint64
sys_sigsend(void)
{
  int pid;
  int signum;

  argint(0, &pid);
  argint(1, &signum);
  return sigsend(pid, signum);
}

uint64
sys_clone(void)
{
  uint64 fn, stack, arg;

  argaddr(0, &fn);
  argaddr(1, &stack);
  argaddr(2, &arg);
  return kclone(fn, stack, arg);
}

uint64
sys_join(void)
{
  int tid;

  argint(0, &tid);
  return kjoin(tid);
}

uint64
sys_msgsend(void)
{
  int pid;
  uint64 msg_addr;

  argint(0, &pid);
  argaddr(1, &msg_addr);
  return msgsend(pid, (char *)msg_addr);
}

uint64
sys_msgrecv(void)
{
  uint64 buf_addr;

  argaddr(0, &buf_addr);
  return msgrecv((char *)buf_addr);
}

uint64
sys_sem_init(void)
{
  int semid, value;

  argint(0, &semid);
  argint(1, &value);
  return ksem_init(semid, value);
}

uint64
sys_sem_wait(void)
{
  int semid;

  argint(0, &semid);
  return ksem_wait(semid);
}

uint64
sys_sem_post(void)
{
  int semid;

  argint(0, &semid);
  return ksem_post(semid);
}

uint64
sys_setpriority(void)
{
  int pid;
  int priority;

  argint(0, &pid);
  argint(1, &priority);
  return setpriority(pid, priority);
}

uint64
sys_getpriority(void)
{
  int pid;

  argint(0, &pid);
  return getpriority(pid);
}

uint64
sys_waitx(void)
{
  uint64 rtime;
  uint64 wtime;

  argaddr(0, &rtime);
  argaddr(1, &wtime);
  return waitx(rtime, wtime);
}

uint64
sys_getpinfo(void)
{
  uint64 addr;
  struct pinfo info;

  argaddr(0, &addr);
  if(getpinfo(&info) < 0)
    return -1;

  if(copyout(myproc()->pagetable, addr, (char *)&info, sizeof(info)) < 0)
    return -1;

  return 0;
}
