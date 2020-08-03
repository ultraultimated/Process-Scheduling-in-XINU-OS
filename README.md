# Process-Scheduling-in-XINU-OS

## Random Scheduling
Random Scheduler The first scheduling policy is a random scheduler. This scheduler will choose at random a process eligible for execution. The probability of choosing a specific process will be proportional to its priority. For example, assume that there are three processes eligible for execution P1, P2, and P3 (each of them is either in the ready list or is the current process) with priorities 30, 20, and 10. The total-sum of those priorities is 60. The goal of the random scheduler is to allocate the CPU with a probability of 30/60 to P1, 20/60 to P2, and 10/60 to P3.
To accomplish this, each time a rescheduling happens, the scheduler will generate a random number between 0 and total-sum minus 1 (0 to 59 in this case). If the random number is less than the priority of the first process in the list of processes eligible for execution, then the scheduler schedules this process for execution. Otherwise, the random number is decreased by priority units and it goes to the next process in the list and so on. For example, assume that the random number generated was 38, and the first process in the list is P1. As 38 is not less than 30, we decrease the number 38 by 30 and we go to the next process in the list. As 8 is less than 20, then the scheduler will choose P2 for execution. In your implementation you may use the srand() and rand() functions, as implemented in ./lib/libxc/rand.c.

## Linux-like Scheduler (based loosely on the 2.2 Linux kernel) 
This scheduling algorithm tries to loosely emulate the Linux scheduler in 2.2 kernel. In this assignment, we consider all the processes “conventional processes” and uses the policies regarding SCHED_OTHER scheduling class within the 2.2 kernel. With this algorithm, the CPU time is divided into epochs. In each epoch, every process has a specified time quantum, whose duration is computed at the beginning of the epoch. An epoch will end when all the runnable processes have used up their quantum. If a process has used up its quantum, it will not be scheduled until the next epoch starts, but a process can be selected many times during the epoch if it has not used up its quantum.
When a new epoch starts, the scheduler will recalculate the time quantum of all processes (including blocked ones). This way, a blocked process will start in the epoch when it becomes runnable again. New processes created in the middle of an epoch will wait till the next epoch, however. For a process that has never executed or has exhausted its time quantum in the previous epoch, its new quantum value is set to its process priority (i.e., quantum = priority). A quantum of 10 allows a process to execute for 10 ticks (10 timer interrupts) within an epoch. For a process that did not get to use up its previously assigned quantum, we allow part of the unused quantum to be carried over to the new epoch. Suppose for each process, a variable counter describes how many ticks are left from its quantum, then at the beginning of the next epoch, quantum = floor(counter/2) + priority. For example, a counter of 5 and a priority of 10 will produce a new quantum value of 12. During each epoch, runnable processes are scheduled according to their goodness. For processes that have used up their quantum, their goodness value is 0. For other runnable processes, their goodness value is set considering both their priority and the amount of quantum allocation left: goodness = counter + priority. Note that round-robin is used among processes with equal goodness.

The priority can be changed by explicitly specifying the priority of the process during the create() system call or through the chprio() function. Priority changes made in the middle of an epoch, however, will only take effect in the next epoch. An example of how processes should be scheduled under this scheduler is as follows:

If there are processes P1, P2, P3 with time quantum 10,20,15 then the epoch would be equal to 10+20+15=45 and the possible schedule (with quantum duration specified in the braces) can be: P2(20), P3(15), P1(10), P2(20) ,P3(15), P1(10) but not: P2(20), P3(15), P2(20), P1(10).
