#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
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
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
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

uint64
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


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 va, abits, mask = 0;
  int n;
  pagetable_t pagetable = myproc()->pagetable;

  // vmprint(pagetable);

  if(argaddr(0, &va) < 0 || argint(1, &n) < 0 || argaddr(2, &abits) < 0) {
    printf("pgaccess: args failed\n");
    return -1;
  }

  if (n > sizeof(mask) * 8) {
    printf("pgaccess: n is too big\n");
    return -1;
  }

  for (int m = 0; m < n; m++) {
    pte_t *pte = walk(pagetable, va + m * PGSIZE, 0);
    if (pte && (*pte & PTE_A)) {
      mask = mask | 1 << m;
      *pte &= ~PTE_A; //why?
    }
  }

  if (copyout(pagetable, abits, (char*)&mask, sizeof(mask)) < 0) {
    printf("pgaccess: copyout failed\n");
    return -1;
  }

  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
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
