#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h> 
#include <stdio.h> 
#include "errmacros.h"

#define FIFO_NAME 	"MYFIFO" 

int main(void) { // FIFO reader
  FILE *fp; 
  int result;
  char *send_buf; 
  int i = LOOPS;
  int *p;
p=NULL;
  
  /* Create the FIFO if it does not exist */ 
  result = mkfifo(FIFO_NAME, 0666);
  CHECK_MKFIFO(result); 	
  
  fp = fopen(FIFO_NAME, "w"); 
  printf("syncing with reader ok\n");
  FILE_OPEN_ERROR(fp);
                                                                                
  while ( i-- )
  {
    asprintf( &send_buf, "Test message %d\n", LOOPS-i );
    if ( fputs( send_buf, fp ) == EOF ) //fputs函数的功能是向指定的文件写入一个字符串，
    {
      fprintf( stderr, "Error writing data to fifo\n");
      exit( EXIT_FAILURE );
    } 
    //FFLUSH_ERROR(fflush(fp));//fflush()函数将清除由stream指向的文件缓冲区里的内容，常用于写完一些数据后，立即用该函数清除缓冲区。
    printf("Message send: %s", send_buf); 
    free( send_buf );
    sleep(1);
if(i==2){*p=0;}    // 在这里故意出错，所以read那一方没有收到data，因为data还在缓冲区，没有被写进去， 这就是为什么需要fflush的原因
  } 
  
  result = fclose( fp );
  FILE_CLOSE_ERROR(result);
  
  exit(EXIT_SUCCESS);
}
