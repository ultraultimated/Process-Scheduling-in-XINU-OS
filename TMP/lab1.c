#include <kernel.h>
#include <proc.h>
#include <q.h>


schedulerClass = 0;	// setting the default scheduler class as 0 


void setschedclass(int class){
	schedulerClass  = class; // setting the scheduler class
}

int getschedclass(){
	return(schedulerClass ); // getting the scheduler class
}
