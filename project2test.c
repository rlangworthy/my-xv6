#include "types.h"
#include "stat.h"
#include "user.h"

//Testing to see if the trap table resets by brute forcing through the process table.
//There are a total number of 64 available processes.
//I'm going to have the parent fork 100 times, so I'm guaranteed to get the same proc struct more than once as a child.
/*int tableResetTest(void){
	int pid;
	int i;
	for(i = 0; i < 100; i++){
		pid = fork()
		if(pid > 0){
			wait();
		}
		if(pid == 0){
			struct proc *p = myproc();
			if(p->traps[SYS_close]!=0){
				printf(1, "failed on run %d\n", i);
				exit();
			}
			close(-1);
			if(p->traps[SYS_close]!=1){
				printf(1, "failed on run %d\n", i);
				exit();
			}
			exit();
		}
	}
	return 1;
}
*/

int main(void){

	tableResetTest();


    countTraps();
    exit();
}