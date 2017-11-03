#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#define DEVNAME "/dev/hello"

 int main()
{
  char alpha[27];
  int fd,i;

  memset(alpha, 0, 27);
  for(i = 0; i < 26; i++)
         alpha[i] = 'A' + i;

  fd = open(DEVNAME, O_RDWR);
  if(fd == -1)
  {
         printf("file %s is opening......failure!", DEVNAME);
  }
  else
  {
         printf("file %s is opening......successfully!\nits fd is %d\n", DEVNAME, fd);
  }

  getchar();
  printf("write A-Z to kernel......\n");
  write(fd, alpha, 26);

  getchar();
  printf("read datas from kernel.......\n");
  read(fd, alpha, 26);
  printf("%s\n", alpha);

  getchar();
  close(fd);

  return 0;
}