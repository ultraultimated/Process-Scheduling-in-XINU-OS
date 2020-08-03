#ifndef _SCHED_H_
#define _SCHED_H_

#define RANDOMSCHED 1
#define LINUXSCHED 2

extern int schedulerClass;

extern void setschedclass(int sched_class);
extern int getschedclass();

#endif
