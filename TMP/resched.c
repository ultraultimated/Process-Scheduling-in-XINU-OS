/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>
unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

int resched() {
	int sched = getschedclass();
	if(sched == LINUXSCHED)
	{
		Linux_resched();	
	}
	else if(sched == RANDOMSCHED){
		random_resched();
	}
	else{
		xinu_default();
	}
}
int xinu_default(){
	register struct pentry *optr, *nptr;
if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&(lastkey(rdytail)<optr->pprio))
{
return(OK);
}
/* force context switch */
if (optr->pstate == PRCURR)
{
optr->pstate = PRREADY;
insert(currpid,rdyhead,optr->pprio);
}
/* remove highest priority process at end of ready list */
nptr = &proctab[ (currpid = getlast(rdytail)) ];
nptr->pstate = PRCURR; /* mark it currently running */
#ifdef RTCLOCK
preempt = QUANTUM; /* reset preemption counter */
#endif

ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

/* The OLD process returns here when resumed. */
return OK;
}
					
int Linux_resched(){
		register struct	pentry	*optr; 
		register struct pentry  *nptr;
		int max_goodness = 0;
		int np = 0;
		int temp = 0;		
		int priority_value=0;				
		int current_priority;
		priority_value++;
		int is_initialized = 0;
		while(!is_initialized){
			optr = &proctab[currpid];
			int j = optr->counter;
			temp = optr->goodness;			
			current_priority = temp - j;
			temp = current_priority;
			is_initialized = 1;
			if(preempt!=0)
				optr->goodness = temp + preempt;
					
			optr->counter = preempt;
		}
		if ( currpid == NULLPROC ){
			
			optr->goodness = 0, optr->counter = 0;
		}
		if( optr->counter <=0 )
		{
			optr->goodness = 0, optr->counter = 0;
		}
		int i;
		for(i = q[rdytail].qprev; i < rdyhead; i = q[i].qprev){
			 do{
				if(proctab[i].goodness > max_goodness){
					max_goodness = proctab[i].goodness;
					np = i;
				}
				else
					continue;
			}while(proctab[i].goodness > max_goodness);
		}

		if (optr->counter == 0) 
		{			
			if(max_goodness == 0){
				int priority_value = new_epoch();
				if(priority_value>0)
					preempt = optr->counter;
				if(priority_value > 0){
					if(currpid == NULLPROC)
					{
						return OK;
					}
					else
					{
						#ifndef 	TCLOCK
							preempt = QUANTUM;
						#endif
					}
				}
			}
		
		}
		else if (optr->goodness > 0){
			if(optr->goodness >= max_goodness){
				if (optr->pstate == PRCURR)
				{

					preempt = optr->counter;
					return(OK);
				}
				else 			
				{
					temp = optr->counter;
				}
			}
		}		
		if (optr->pstate == PRCURR) {
				optr->pstate = PRREADY;
				insert(currpid, rdyhead, optr->pprio);
		}

		nptr = &proctab[np];
		nptr->pstate = PRCURR;
		dequeue(np);
		currpid = np;
		preempt = nptr->counter;
		
		ctxsw((int) &optr->pesp, (int) optr->pirmask, (int) &nptr->pesp, (int) nptr->pirmask);
		return(OK);
}
int new_epoch(){
			int priority_value = 1;
			struct pentry *ptr;
			int i;
			for(i = 1; i < NPROC; i++){
				
				ptr = &proctab[i];
				if (ptr->pstate != PRFREE){
					ptr->counter = ptr->pprio;
					ptr->counter += (int)((ptr->counter))>>1;
					priority_value++;
				}
				ptr->goodness =   ptr->pprio;
				ptr->goodness += ptr->counter;				
			}
			return priority_value;		
}                 
int random_resched(){
	register struct	pentry	*optr, *nptr;	/* pointer to old process entry */
       	
	if((optr= &proctab[currpid])->pstate == PRCURR){
		optr->pstate = PRREADY;
	        insert(currpid,rdyhead,optr->pprio);
	}
	int sum = 0;
	int i;
        for(i = q[rdytail].qprev; i != rdyhead; i = q[i].qprev) {
                sum += q[i].qkey;
        }
	currpid = 0;
        if(sum > 0){
                int random_number = rand()%sum;
                for(i = q[rdytail].qprev; i != rdyhead; i = q[i].qprev){
			currpid = i;
			if(random_number > q[i].qkey){
				random_number -= q[i].qkey;
			}
			else{
				break;
			}

		}
	}
	
	dequeue(currpid);
	nptr = &proctab[currpid];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif	
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;

}

