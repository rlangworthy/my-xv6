#include "types.h"
#include "stat.h"
#include "user.h"

//Testing to see if the trap table resets by brute forcing through the process table.
//There are a total number of 64 available processes.
//I'm going to have the parent fork 100 times, so I'm guaranteed to get the same proc struct more than once as a child.
int tableResetTest(void){
	int pid;
	int i;
	for(i = 0; i < 100; i++){
		pid = fork();
		if(pid > 0){
			wait();
		}
		if(pid == 0){
			uptime();
			if(i == 3){
				printf(1, "Child 3 trap count \n");
				countTraps();
			}
			if(i == 99){
				printf(1, "Child 100 trap count \n");
				countTraps();
			}
			exit();
		}
	}
	printf(1, "Parent trap count \n");
	countTraps();
	return 1;
}

int main(void){

	tableResetTest();


    //countTraps();
    exit();
}