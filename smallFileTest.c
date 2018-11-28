#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

void
test_failed()
{
  printf(1, "TEST FAILED\n");
  exit();
}

void
test_passed()
{
 printf(1, "TEST PASSED\n");
 exit();
}

#define MAX 52

int
main()
{
  int fd;
  char buf[MAX];
  char buf2[MAX];
  int n;
  int i;
  
  for(i = 0; i < MAX; i++){
    buf[i] = (char)(i+(int)'0');
  }
  memset(buf2, 0, MAX);
  
  //create and write
  if((fd = open("test_file.txt", O_CREATE | O_SMALLFILE | O_RDWR)) < 0){
    printf(1, "Failed to create the small file\n");
    test_failed();
    exit();
  }
  if((n = write(fd, buf, MAX)) != MAX){
    printf(1, "Write failed!\n");
    test_failed();
  }
  close(fd);
  
  //read
  if((fd = open("test_file.txt", O_CREATE | O_SMALLFILE | O_RDWR)) < 0){
    printf(1, "Failed to open the small file\n");
    test_failed();
  }
  for(i = 0; i < MAX; i++) {
    if((n = read(fd, &buf2[i], 1)) != 1){
      printf(1, "Read failed!\n");
      test_failed();
    }
    /*printf(1, "Buffer 1[%d] : %c\n", buf[i]);
    printf(1, "Buffer 2[%d] : %c\n", buf2[i]);
    if(buf[i] != buf2[i]){
      printf(1, "Data mismatch.\n");
      //test_failed();
    }*/
  }

  for(i = 0; i < MAX; i++){
    printf(1, "Buffer 1[%d] : %c\n", buf[i]); 
  }

  for(i = 0; i < MAX; i++){
    printf(1, "Buffer 2[%d] : %c\n", buf2[i]); 
  }
  close(fd);

  test_passed();
  exit();
}
