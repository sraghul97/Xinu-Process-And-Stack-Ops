/* fork.c - fork, newpid */

#include <xinu.h>

//local	int newpid();

/*------------------------------------------------------------------------
 *  fork  -  fork a process to start running on x86
 *------------------------------------------------------------------------
 */
pid32	fork(void)
	 
{
	uint32	ssize;		/* Stack size in bytes		*/

	//Parent Process Parameters
	uint32 *parent_ebp;
	uint32 *parent_esp;
	pid32 parent_pid;
	struct	procent	*parent_prptr;
	uint32		*parent_saddr;		
	unsigned long   *parent_esp_ulong, *parent_ebp_ulong;

	uint32		savsp, *pushsp;
	intmask 	mask;    	/* Interrupt mask		*/
	pid32		pid;		/* Stores new process id	*/
	struct	procent	*prptr;		/* Pointer to proc. table entry */
	int32		i;
	//uint32		*a;		/* Points to list of args	*/
	uint32		*saddr;		/* Stack address		*/
	uint32 *eip;	//	Child pointer from which child process begins execution

	signed long child_stack_offset;

	mask = disable();
	parent_pid = (pid32)getpid();
	parent_prptr =  &proctab[parent_pid];
	ssize = parent_prptr->prstklen;
	parent_saddr = parent_prptr->prstkbase;
    asm("movl %%ebp, %0\n" :"=r"(parent_ebp_ulong));
	asm("movl %%esp, %0\n" :"=r"(parent_esp_ulong));
	parent_ebp = (uint32 *)parent_ebp_ulong;
	parent_esp = (uint32 *)parent_esp_ulong;
		
	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (uint32) roundmb(ssize);
	if ( ((pid=newpid()) == SYSERR) ||
	     ((saddr = (uint32 *)getstk(ssize)) == (uint32 *)SYSERR) ) {
		restore(mask);
		return SYSERR;
	}

	prcount++;
	prptr = &proctab[pid];

	/* Initialize process table entry for CHILD process */
	prptr->prstate = PR_READY;	/* Initial state is READY not SUSPEND	*/
	prptr->prprio = parent_prptr->prprio;
	prptr->prstkbase = (char *)saddr;
	prptr->prstklen = ssize;
	prptr->prname[PNMLEN-1] = NULLCH;
	for (i=0 ; i<PNMLEN-1 && (prptr->prname[i]=parent_prptr->prname[i])!=NULLCH; i++)   //Same name as parent process
		;
	prptr->prsem = -1;
	prptr->prparent = (pid32)getpid();
	prptr->prhasmsg = FALSE;

	/* Set up stdin, stdout, and stderr descriptors for the shell	*/
	prptr->prdesc[0] = CONSOLE;
	prptr->prdesc[1] = CONSOLE;
	prptr->prdesc[2] = CONSOLE;
	prptr->user_process = FALSE;  //SYSTEM PROCESS

		/* Initialize stack as if the process was called		*/
		
		*saddr = STACKMAGIC;
		savsp = (uint32)saddr;
		
		child_stack_offset = (long)savsp - (long)(uint32)(parent_prptr->prstkbase);

		while(parent_saddr > parent_esp)
		{
			*--saddr = *--parent_saddr;	//Copying all stack contents from parent to child
		}

		while (*parent_ebp != STACKMAGIC)	//setting all child base pointers correctly by adding offsets
		{
			*(uint32 *)((uint32)parent_ebp + child_stack_offset) = (uint32 *)((uint32)(*parent_ebp) + child_stack_offset);
			parent_ebp = (uint32 *)(uint32)(*parent_ebp);
		}
		
		asm ("mov $., %0" : "=r"(eip));	//Retriving Instruction address from which child should start executing (PC+4)	
		if (parent_pid == getpid())
		{
		
		///* Push arguments */
		//a = (uint32 *)(&nargs + 1);	/* Start of args		*/
		//a += nargs -1;			/* Last argument		*/
		//for ( ; nargs > 0 ; nargs--)	/* Machine dependent; copy args	*/
		//	*--saddr = *a--;	/* onto created process's stack	*/
		//*--saddr = (long)INITRET;	/* Push on return address	*/
		
		/* The following entries on the stack must match what ctxsw	*/
		/*   expects a saved process state to contain: ret address,	*/
		/*   ebp, interrupt mask, flags, registers, and an old SP	*/
		*--saddr = eip;	/* Make the stack look like it's*/
						/*   half-way through a call to	*/
						/*   ctxsw that "returns" to the*/
						/*   new process		*/
		*--saddr = ((uint32)parent_ebp_ulong) + child_stack_offset;		/* This will be register ebp	*/
						/*   for process exit		*/
		savsp = (uint32) saddr;		/* Start of frame for ctxsw	*/
		*--saddr = 0x00000200;		/* New process runs with	*/
						/*   interrupts enabled		*/

		/* Basically, the following emulates an x86 "pushal" instruction*/

		*--saddr = 0;			/* %eax */
		*--saddr = 0;			/* %ecx */
		*--saddr = 0;			/* %edx */
		*--saddr = 0;			/* %ebx */
		*--saddr = 0;			/* %esp; value filled in below	*/
		pushsp = saddr;			/* Remember this location	*/
		*--saddr = savsp;		/* %ebp (while finishing ctxsw)	*/
		*--saddr = 0;			/* %esi */
		*--saddr = 0;			/* %edi */
		*pushsp = (unsigned long) (prptr->prstkptr = (char *)saddr);
		insert(pid,readylist,prptr->prprio);	//	inserting child proces to ready list
		restore(mask);
		return pid;
	}
	else	//Child Process
	{
		return NPROC;  
	}	
}

///*------------------------------------------------------------------------
// *  newpid  -  Obtain a new (free) process ID
//*------------------------------------------------------------------------
//*/
///*local	pid32	newpid(void)
//{
//	uint32	i;			/* Iterate through all processes*/
//	static	pid32 nextpid = 1;	/* Position in table to try or	*/
//					/*   one beyond end of table	*/
//
//	/* Check all NPROC slots */
//
//	for (i = 0; i < NPROC; i++) {
//		nextpid %= NPROC;	/* Wrap around to beginning */
//		if (proctab[nextpid].prstate == PR_FREE) {
//			return nextpid++;
//		} else {
//			nextpid++;
//		}
//	}
//	return (pid32) SYSERR;
//}
