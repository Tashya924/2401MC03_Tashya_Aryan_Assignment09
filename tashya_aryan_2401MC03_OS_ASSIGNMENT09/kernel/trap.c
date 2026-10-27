#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed)
      exit();
    myproc()->tf = tf;
    syscall();
    if(myproc()->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpuid() == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;

  // Assignment 9 Q2: lazy allocation page-fault handler.
  case T_PGFLT:
    if(myproc() != 0 && (tf->cs&3) == DPL_USER){
      uint va = rcr2();
      struct proc *p = myproc();
      pte_t *pte;
      char *mem;
      // Valid lazy fault: address below sz and no present mapping.
      // (A present PTE means a protection fault, e.g. the stack guard
      // page, which must NOT be "fixed" or we would loop forever.)
      if(va < p->sz){
        pte = walkpgdir(p->pgdir, (void*)va, 0);
        if(pte == 0 || !(*pte & PTE_P)){
          mem = kalloc();
          if(mem == 0){
            cprintf("pid %d: lazy alloc out of memory at 0x%x--kill proc\n",
                    p->pid, va);
            p->killed = 1;
            break;
          }
          memset(mem, 0, PGSIZE);
          if(mappages(p->pgdir, (char*)PGROUNDDOWN(va), PGSIZE,
                      V2P(mem), PTE_W|PTE_U) < 0){
            kfree(mem);   // do not leak the page if mapping fails
            cprintf("pid %d: lazy alloc mapping failed at 0x%x--kill proc\n",
                    p->pid, va);
            p->killed = 1;
            break;
          }
          p->nfaults++;   // count only successful lazy-allocation faults
          break;          // resume: faulting instruction is re-executed
        }
      }
      // Invalid address (>= sz) or protection fault: kill only this process.
      cprintf("pid %d: page fault at invalid address 0x%x--kill proc\n",
              p->pid, va);
      p->killed = 1;
      break;
    }
    // Page fault in kernel mode: fall through to the generic panic path.
    // fallthrough
  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpuid(), tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno,
            tf->err, cpuid(), tf->eip, rcr2());
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();
}
