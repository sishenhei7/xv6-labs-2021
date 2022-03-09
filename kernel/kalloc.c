// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct {
  struct spinlock lock;
  int count[((PHYSTOP - KERNBASE) / PGSIZE)];
} pcount;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&pcount.lock, "pcount");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    pcount.count[((uint64)p - KERNBASE) / PGSIZE] = 1;
    kfree(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  minuspcount((uint64)pa);
  if (getpcount((uint64)pa) > 0) {
    return;
  }

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    addpcount((uint64)r);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

int
getpcount(uint64 pa) {
  return pcount.count[(pa - KERNBASE) / PGSIZE];
}

// add page count
void
addpcount(uint64 pa)
{
  pcount.count[(pa - KERNBASE) / PGSIZE]++;
}

// minus page count
void
minuspcount(uint64 pa)
{
  pcount.count[(pa - KERNBASE) / PGSIZE]--;
}

// handle cow page fault
int
handlecow(pagetable_t pagetable, uint64 va) {
  pte_t *pte;
  char *mem;

  if((pte = walk(pagetable, va, 0)) == 0) {
    printf("handlecow: pte should exist\n");
    return -1;
  }

  if ((*pte & PTE_V) == 0) {
    printf("handlecow: not valid\n");
    return -1;
  }

  if ((*pte & PTE_U) == 0) {
    printf("handlecow: not user\n");
    return -1;
  }

  if ((*pte & PTE_COW) == 0) {
    return 0;
  }

  uint64 pa = PTE2PA(*pte);

  // 这里需要锁，确保判断count之后立即分配内存并且更新count
  acquire(&pcount.lock);
  if (getpcount(pa) > 1) {
    if((mem = kalloc()) == 0) {
      release(&pcount.lock);
      printf("handlecow: can not kalloc\n");
      return -1;
    }

    memmove(mem, (char*)pa, PGSIZE);
    minuspcount(pa);
    *pte = PA2PTE(mem) | PTE_FLAGS(*pte) | PTE_W;
  }
  release(&pcount.lock);

  *pte |= PTE_W;
  *pte &= ~PTE_COW;
  return 0;
}