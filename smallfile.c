
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"


void test_failed(){
	printf(1, "TEST FAILED\n");
//	exit();
}

void test_passed(){
	printf(1, "TEST PASSED\n");
//	exit();
	
}


#define MAX 25
#define NBLOCKS (NDIRECT+1)  //12+1
#define SIZE (NBLOCKS*4)   //52


void test1(){
	printf(1,"===Test 1. Trying to create a small file smallFile.txt===\n");
	
	char *fileName = "test_file.txt";
	int fd; // Integer for file descriptor returned by open() call 
	fd = open(fileName,O_CREATE | O_SMALLFILE | O_RDWR); 
	printf(1, "fd is %d\n",fd);  
	// ORDWR - open file for reading and writing
	

	if (fd==-1){
		printf(1,"Failed to create a small file\n");
		test_failed();
	}
	if (fd>0){
		printf(1, "Opening  smallFileTest.txt\n");
		test_passed();
		
	}	
	close(fd);
}

void test2(){
	printf(1,"===Test 2. Trying to write into a small file smallFile.txt===\n");
        
        char *fileName = "test_file.txt";
        int fd; // Integer for file descriptor returned by open() call 
 	 char buf[MAX];
  //	char buf2[MAX];
  	int n;
  	int i;
  
  	for(i = 0; i < MAX; i++){
    		buf[i] = (char)(i+(int)'0');
 	 }	
	printf(1, "Size of buffer is %d\n",strlen(buf));

	fd = open(fileName, O_CREATE | O_SMALLFILE | O_RDWR);
	if (fd>0){
		n = write(fd, buf, MAX);
		if (n==MAX){
			printf(1,"Wrote 25 bytes into a small file\n");
			test_passed();
		}
		else{
			printf(1,"Failed to write 25 bytes into a small file\n");
			test_failed();
			}
		}
        close(fd);
}


void test3(){
		printf(1,"===Test 3. Reading smallFile.txt after write===\n");
		char *fileName = "test_file.txt";
 		char buf2[MAX];
		struct stat st;

        	int fd, n; // Integer for file descriptor returned by open() call
		if((fd = open(fileName, O_CREATE | O_SMALLFILE | O_RDWR)) < 0){
   			printf(1, "Failed to open the small file\n");
    			test_failed();
  		}
		else{
			printf(1, "Opened the file after write\n");
			test_passed();
		}	
		n = read(fd, buf2, MAX+10);
		//attempts to read 35 bytes but read 25
	//	printf(1, "Number of bytes read : %d\n", n);
		if(n != MAX){
		    printf(1, "Read failed!\n");
    		    test_failed();
  		}
		else{
			printf(1, "Number of bytes read : %d\n", n);
			printf(1, "String Read : %s\n", buf2);
			test_passed();
			//close(fd);
		}

		if(fstat(fd, &st) < 0){
   			 printf(1, "Failed to get stat on the small file\n");
    			test_failed();
 		 }
	
}



int main(){
	printf(1, "==================================\n");
	printf(1, "Test data for small file project\n");	

	test1();
	test2();
	test3();
	printf(1, "DONE\n");	
	exit();



}


