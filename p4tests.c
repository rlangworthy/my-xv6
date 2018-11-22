#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

char buf[8192];
char name[3];
int stdout = 1;

void
opentest(void)
{
  int fd;

  printf(stdout, "open test\n");
  fd = open("echo", O_CREATE & O_SMALL);
  if(fd < 0){
    printf(stdout, "open echo failed!\n");
    exit();
  }
  close(fd);
  fd = open("echo", O_RDWR);
  if(fd >= 0){
    printf(stdout, "open echo succeeded!\n");
  }
  char *w = "asdfasdf";
  char r[strlen(w)];
  write(fd, w,strlen(w));
  printf(stdout, "write\n");
  read(fd, r, strlen(r));
  printf(stdout, "read %s\n", r);
  printf(stdout, "open test ok\n");
}

int
main(int argc, char const *argv[])
{
    opentest();

    return 0;
}
