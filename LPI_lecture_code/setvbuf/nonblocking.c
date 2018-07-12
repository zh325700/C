
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
  char a; 
  int flags;
  int result;
  
  flags = fcntl(0, F_GETFL);    // O = stdin file descriptor
  if (flags == -1)
  {
    perror("fcntl error\n");
    exit(-1);
  }
  flags |= O_NONBLOCK;
  if (fcntl(0, F_SETFL, flags) == -1)
  {
    perror("fcntl error\n");
    exit(-1);
  }
  
  while ( 1 )
  {
    
    result = scanf("%c", &a);  // use scanf in non-blocking mode
    if ( result == 1 )
    {
      printf("\ninput char: %c", a);
    } 
    else 
    {
      printf("\nno input");
      sleep(1);
    }
  }
  
  return 1;
}
