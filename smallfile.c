
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"


void test_failed(int n){
	printf(1, "TEST %d FAILED\n", n);
}

void test_passed(int n){
	printf(1, "TEST %d PASSED\n", n);	
}

int test_open(char *fileName, int flags, int test){
	int fd;
	if((fd = open(fileName, flags)) < 0){
   		printf(1, "Failed to open the small file\n");
    	test_failed(test);
		return -1;
  	}else{
		printf(1, "Opened %s\n", fileName);
		return fd;
	}
}
void initialize_buf(char *buf, int size){
	int i;
	for(i = 0; i < size; i++){
		buf[i] = (char)(i+(int)'0');
	}
}

#define MAX 25
#define NBLOCKS (NDIRECT+1)  //12+1
#define SIZE (NBLOCKS*4)   //52


int test1(){
	printf(1,"===Test 1. Trying to create a small file test_file.txt===\n");
	
	char *fileName = "test_file.txt";
	int fd; // Integer for file descriptor returned by open() call 
	// ORDWR - open file for reading and writing
	if ((fd=test_open(fileName, O_CREATE | O_SMALLFILE | O_RDWR, 1)) > 0){
		test_passed(1);
		close(fd);
		return -1;
	}
	return -1;
}

int test2(){
	printf(1,"===Test 2. Trying to write into a small file test_file.txt===\n");
        
    char *fileName = "test_file.txt";
    int fd; // Integer for file descriptor returned by open() call 
 	char buf[SIZE];
  	int n;
  	int i;
  
	initialize_buf(buf, SIZE);
	printf(1, "Size of buffer is %d\n",SIZE);

	fd = open_test(fileName, O_RDWR, 2);
	if (fd>0){
		n = write(fd, buf, MAX);
		if (n==MAX){
			printf(1,"Wrote 25 bytes into a small file\n");
			test_passed(2);
		}else{
			printf(1,"Failed to write 25 bytes into a small file\n");
			test_failed(2);
			}
		close(fd);
		return 1;
	}
	return -1;
}

int test3(){
		printf(1,"===Test 3. Reading test_file.txt after write===\n");
		char *fileName = "test_file.txt";
 		char buf2[SIZE];
		memset(buf2, 0, SIZE);
		//struct stat st;
		int fd, n; // Integer for file descriptor returned by open() call
		
		if((fd = open_test(fileName, O_RDWR, 3)) < 0)
			return -1;

		n = read(fd, buf2, MAX+10);
		//attempts to read 35 bytes but read 25
		//printf(1, "Number of bytes read : %d\n", n);
		if(n != MAX){
		    printf(1, "Read failed!\n");
    		test_failed(3);
			return -1;
  		}
		else{
			printf(1, "Number of bytes read : %d\n", n);
			test_passed(3);
			close(fd);
			return 1;
		}
}

int test4(){
	printf(1,"===Test 4. fstat returns correctly===\n");
	char *fileName = "test_file.txt";
	struct stat st;
	int fd;

	if((fd = open_test(fileName, O_RDWR, 4)) < 0)
		return -1;
			
	if(fstat(fd, &st) < 0){
   		printf(1, "Failed to get stat on the small file\n");
    	test_failed(4);
		return -1;
 	}else{
		if(st.type != T_SMALLFILE || st.size != MAX){
			printf(1, "Fstat returned incorrect values\n");
			test_failed(4);
			return -1;
		}
		printf(1, "File Type: %d \nFile Size: %d\n", st.type, st.size);
		test_passed(4);
		close(fd);
		return 1;
	 }
}

int test5(){
	printf(1,"===Test 5. Reading and Writing Max size===\n");
	char *fileName = "test_file.txt";
	char buf1[SIZE], buf2[SIZE];
	//struct stat st;
	int fd, n, i; // Integer for file descriptor returned by open() call
	
	initialize_buf(buf1, SIZE);
	
	if((fd = open_test(fileName, O_RDWR, 5)) < 0)
		return -1;
	
	n = write(fd, buf1, SIZE);

	if(n != SIZE){
		printf(1, "Write failed!\n");
		test_failed(5);
		return -1;
	}
	close(fd);
	fd = open(fileName, O_RDWR);
	if((n=read(fd, buf2, SIZE)!=SIZE)){
		printf(1, "Read failed!\n");
		test_failed(5);
		return -1;
	}else{
		//check read values vs write values
		for(i = 0; i < SIZE; i++){
    		if(buf1[i] != buf2[i]){
				printf(1, "Read not equal to Write\n");
				test_failed(5);
				return -1;
			}
 	 	}
		test_passed(5);
		close(fd);
		return 1;
	}
}

int test6(){
	printf(1,"===Test 6. Reading and Writing past Max size===\n");
	char *fileName = "test_file.txt";
	char buf1[SIZE+1], buf2[SIZE+1];
	struct stat st;
	int fd, n, i; // Integer for file descriptor returned by open() call
	
	initialize_buf(buf1, SIZE +1);
	printf(1, "Size of buffer is %d\n",(SIZE+1));
	
	if((fd = open_test(fileName, O_RDWR, 6)) < 0){
		return -1;

	n = write(fd, buf1, SIZE+1);
	//attempts to write 53
	//printf(1, "Number of bytes read : %d\n", n);
	if(n != SIZE+1){
		printf(1, "Write failed!\n");
		test_failed(6);
		return -1;
	}
	close(fd);
	fd = open(fileName, O_RDWR);
	if((n=read(fd, buf2, SIZE+1)!= SIZE+1 )){
		printf(1, "Read failed!\n");
		test_failed(6);
		return -1;
	}else{
		for(i = 0; i < SIZE +1; i++){
    		if(buf1[i] != buf2[i]){
				printf(1, "Read not equal to Write\n");
				test_failed(6);
				return -1;
			}
 	 	}
	}

	fstat(fd, &st);
	if(st.type!=T_FILE || st.size!= SIZE+1){
		printf(1, "Wrong Stats\n");
		printf(1, "File Type: %d  File Size: %d\n", st.type, st.size);
		test_failed(6);
		return -1;
	}
	printf(1, "Correct Stats\n");
	printf(1, "File Type: %d  File Size: %d\n", st.type, st.size);

	test_passed(6);
	close(fd);
	return 1;
}

int test7(){
	printf(1,"===Test 7. Multiple Writes and Reads===\n");
	char *fileName = "test_file2.txt";
	char buf1[100];
	char buf2[30];
	struct stat st;
	int fd, n, i;
	
	memset(buf1, 0, 100);
	for(i=0;i<29;i++){
		buf2[i] = (char)(1+(int)'a');
	}
	buf2[29] = '\0';

	if((fd = open(fileName, O_RDWR | O_CREATE | O_SMALLFILE)) < 0){
		printf(1, "Failed to open the small file\n");
		test_failed(5);
		return -1;
	}
	else{
		printf(1, "Test file two created\n");
	}
	printf(1, "String length: %d\n", strlen(buf2));
	n = write(fd, buf2, strlen(buf2));
	if(n != strlen(buf2))
		printf(1, "Write Error\n");
	
	if(fstat(fd, &st) < 0){
   		printf(1, "Failed to get stat on the small file\n");
    	test_failed(4);
		return -1;
 	}else{
		if(st.type != T_SMALLFILE || st.size != strlen(buf2)){
			printf(1, "Fstat returned incorrect values\n");
			test_failed(4);
			return -1;
		}
		printf(1, "File Type: %d \nFile Size: %d\n", st.type, st.size);
	 }

	close(fd);
	fd = open(fileName, O_RDWR);
	n = read(fd, buf1, strlen(buf2));
	if(n != strlen(buf2))
		printf(1, "Read Error\n");
	for(i=0;i<strlen(buf2);i++){
		if(buf1[i] != buf2[i])
			printf(1, "Read Error\n");
	}
	n = write(fd, buf2, strlen(buf2));
	if(n != strlen(buf2))
		printf(1, "Write Error\n");
	fstat(fd, &st);
	printf(1, "File Type: %d \nFile Size: %d\n", st.type, st.size);
	close(fd);
	fd = open(fileName, O_RDWR);
	n = read(fd, buf1, 2*strlen(buf2));
	for(i=0;i<strlen(buf2);i++){
		if(buf1[i+strlen(buf2)] != buf2[i])
			printf(1, "Read Error\n");
	}
	return 1;
}
int main(){
	printf(1, "==================================\n");
	printf(1, "Test data for small file project\n");	

	if (!(test1()) || 
	    !(test2()) ||
		!(test3()) ||
		!(test4()) ||
		!(test5()) ||
		!(test6()) ||
		!(test7()))
		printf(1, "Test Failed!\n");
	else
		printf(1, "DONE\n");	

	exit();
}


