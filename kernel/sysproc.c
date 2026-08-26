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
  return 0; // not reached
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

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

/*
 * Returns available virtual address headroom. Does not necessarily
 * mean all is physically available.
 */
uint64
sys_vamemavail(void)
{
  return TRAPFRAME - myproc()->sz;
}

/*
 * Returns available physical memory.
 */
uint64
sys_memavail(void)
{
  return kmemavail();
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
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

/*
 * Sets interpose mask on struct proc. Will later be used to restrict
 * syscalls for the calling process and child.
 *
 * interpose_mask for SYS_open is 1 << 15 or 1 << SYS_open
 *
 * Interposing 2 syscalls SYS_open and SYS_write
 * 1 << SYS_open | 1 << SYS_write
 *
 */
uint64
sys_interpose(void)
{
  // set interpose mask
  struct proc *p = myproc();
  arguint(0, &p->interpose_mask);

  // set path
  if (argstr(1, p->path, MAXPATH) == -1) {
    return -1;
  };

  return 1;
}
