#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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

  argint(0, &n);
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
uint64
sys_info(void)
{
	int param;
	argint(0, &param);
	if(param == 0)
	{
		return(proc_counter());
	}
	else if (param == 1)
	{
		return(count_system_calls);
	}
	else if (param == 2)
	{
		return(count_free_memory());
	}
	return -1;
}
uint64
sys_procinfo(void)
{

	struct pinfo p;
	uint64 point;
	//printf("\nwe are in sysprocinfo\n");
	struct proc *current_proc = myproc();
	//printf("Current process %d, size %d\n", current_proc->pid, current_proc->sz);
	argaddr(0, &point);
	//printf("Typecasted\n");
	p.ppid = current_proc->parent->pid;
	//printf("Got ppid\n");
	p.page_usage = (current_proc->sz * 3)/(10000);
	p.syscall_count = current_proc->sys_counter;
	if(copyout(current_proc->pagetable, point, (char *)&p, sizeof(p)) < 0)
	{
		return -1;
	}
	//printf("copied //data above\n");
	return 0;
}
