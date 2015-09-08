#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 15

int main (int argc, char ** argv){
  int fd;
  int index = 0, nbytes;
  char data[BUF_SIZE];

  if(argc < 2){
    perror("usage: ./test_module /dev/[device_name]");
  }

  fd = open(argv[1],O_RDONLY);

  if( fd == -1){
    perror("Error openning device");
    return -1;
  }
 do{
  nbytes = read(fd,&data[index],BUF_SIZE);
  index++;
  }while (nbytes != 0);

  data[index + 1] = '\0';
  puts(data);
  return 0;
}
