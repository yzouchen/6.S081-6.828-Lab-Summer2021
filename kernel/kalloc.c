// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

//#change#  
//#define NCPU 8      

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct kmem{
  struct spinlock lock;
  struct run *freelist;
} ;

//#change#  
struct kmem kmem_list[NCPU];

//#change#  
//use cpuid() with push_off() and pop_off()
int assemble_cpuid()
{
  push_off();
  int id = cpuid();
  pop_off();
  return id;
}

//#change#
//every cpu init
void
kinit()
{
  for(int i = 0 ; i < NCPU ; i++)
  {
    initlock(&kmem_list[i].lock, "kmem");
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)

//#change#
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
  int id = assemble_cpuid();
  acquire(&kmem_list[id].lock);
  r->next = kmem_list[id].freelist;
  kmem_list[id].freelist = r;
  release(&kmem_list[id].lock);
}


//#add#
void*
steal(int id_full)
{
  struct run *r = 0;
  for(int i = 0 ; i < NCPU ; i++)
  {
    if(i != id_full)
    {
      acquire(&kmem_list[i].lock);    //get lock
      r = kmem_list[i].freelist;    //get freelist
      if(r)
      {
        kmem_list[i].freelist = r->next;    //refresh freelist
        release(&kmem_list[i].lock);
        return (void *) r;
      }
      release(&kmem_list[i].lock);
    }
  }
  return (void*) r;
}


// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.

//#change#
void *
kalloc(void)
{
  struct run *r;
  int id = assemble_cpuid();
  acquire(&kmem_list[id].lock);
  r = kmem_list[id].freelist;
  if(r)
    kmem_list[id].freelist = r->next;
  release(&kmem_list[id].lock);
  if(!r){
    r = steal(id);      //steal freelist
  }
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

