#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

//Testing to see if the trap table resets by brute forcing through the process table.
//There are a total number of 64 available processes.
//I'm going to have the parent fork 100 times, so I'm guaranteed to get the same proc struct more than once as a child.
//If the child only has a count of one uptime trap, then I know it resets the countTraps array
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

int main(int argc, char *argv[]){
    
    if(argc == 1){
    	if(fork() == 0){
    		tableResetTest();
    		exit();
    	}
    	else{
    		wait();
    	}
    	exit();
    }
    
    int i = 0;
    int runs = &argv[1];
    if(runs > 0 && runs < 100)
	    for(i=0;i<runs;i++)
	    	uptime();
	    countTraps();
	    exit();    
	else{
    	printf(1, "Argument too large\n");
    	exit();
    }

}