#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "errmacros.h"

#define LOOPS		5
#define MAX			80
#define CHILD_POS	"\t\t\t"

void run_child ( int pfds[], int exit_code )
{
  pid_t my_pid, parent_pid; 
  my_pid = getpid();
  parent_pid = getppid();
  int i = LOOPS;
  char *send_buf;
  
  close(pfds[0]); // not needed by child
  printf(CHILD_POS "Greetings from child process (pid = %d) of parent (pid = %d) ...\n", my_pid, parent_pid);	
  while ( i-- )
  {
    asprintf( &send_buf, "Test message %d", LOOPS-i );
    write(pfds[1], send_buf, strlen(send_buf)+1 ); // don't forget to send \0!
    printf("Message write to parents %d: \"%s\"\n", my_pid, send_buf); 
    free( send_buf );
    sleep(1);
  } 
  close(pfds[1]); // indicate end of writing
  printf(CHILD_POS "Child process (pid = %d) of parent (pid = %d) is terminating ...\n", my_pid, parent_pid);	
  exit( exit_code ); // this terminates the child process
}

int main( void ) 
{
  pid_t my_pid, child_pid; 
  int pfds[2];
  int result;
  
  my_pid = getpid();
  printf("Parent process (pid = %d) is started ...\n", my_pid);
  
  result = pipe( pfds );
  SYSCALL_ERROR( result );
  
  child_pid = fork();
  SYSCALL_ERROR(child_pid);
  
  if ( child_pid == 0  )
  {  
    run_child( pfds, 0 );
  }
  else
  {  
    // parent’s code
    char recv_buf[MAX];
    
    printf("Parent process (pid = %d) has created child process (pid = %d)...\n", my_pid, child_pid);
    close( pfds[1] ); // not needed by parent
    
    do 
    {
      result = read(pfds[0], recv_buf, MAX);
      SYSCALL_ERROR( result );
      if ( result > 0 )
      { 
	printf("Message received from child %d: \"%s\"\n", child_pid, recv_buf); 
      }
    } while ( result > 0 );
    
    // pipe closed by child: no more reading
    close( pfds[0] );
    
    // wait on termination of child process
    waitpid(child_pid, NULL, 0);
    
    printf("Parent process (pid = %d) is terminating ...\n", my_pid);
  }
  
  exit(EXIT_SUCCESS);
}





