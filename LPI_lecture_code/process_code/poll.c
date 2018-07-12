#define _GNU_SOURCE
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <poll.h>
#include "errmacros.h"

#define LOOPS			3
#define MAX			80
#define CHILD_POS		"\t\t\t"
#define NUM_CHILDREN		3
#define MIN_SLEEP		10000		// in microseconds - min 10ms
#define MAX_SLEEP		5000000		// in microseconds - max 10s

int pipe_fds[NUM_CHILDREN][2];

void run_child ( int child_id, int exit_code )
{
  pid_t my_pid; 
  int i;
  char *send_buf;
  my_pid = getpid();
  int sleep_time;
  
  //srand( time( NULL ) ); // initialize random number generator in every child to avoid the same number sequence
  srand( child_id+my_pid+exit_code );
  
  // close all pipe fds that are not needed by this child
  for ( i = 0; i < NUM_CHILDREN; i++ )
  {
    SYSCALL_ERROR( close( pipe_fds[i][0] ) );
    if ( i != child_id )
    {
      SYSCALL_ERROR( close( pipe_fds[i][1] ) );
    }
  }
  
  i = LOOPS;
  while ( i-- )
  {
    sleep_time = random() % (MAX_SLEEP - MIN_SLEEP) + MIN_SLEEP;
    usleep( sleep_time );
    asprintf( &send_buf, "Test message %d from child %d (pid = %d)", LOOPS-i, child_id, my_pid );
    //printf("\n" CHILD_POS "Child %d sends msg %d", childId, LOOPS-i );
    //fflush(stdin);
    write( pipe_fds[child_id][1], send_buf, strlen(send_buf)+1 ); // don't forget to send \0!
    free( send_buf );
  } 
  
  SYSCALL_ERROR( close( pipe_fds[child_id][1] ) );
  exit( exit_code ); // this terminates the child process
}

int main( void ) 
{
  pid_t my_pid, child_pid[NUM_CHILDREN]; 
  struct pollfd poll_fd[NUM_CHILDREN];
  char recv_buf[MAX];
  int i, alive, result;
  
  my_pid = getpid();
  printf("Parent process (pid = %d) is started ...\n", my_pid);
  
  // create all pipes
  for ( i = 0; i < NUM_CHILDREN; i++ )
  {
    SYSCALL_ERROR( pipe( pipe_fds[i] ) );
  }
  
  // create all child processes
  for ( i = 0; i < NUM_CHILDREN; i++ )
  {		
    child_pid[i] = fork();
    SYSCALL_ERROR(child_pid[i]);
    if ( child_pid[i] == 0  )
    {  
      run_child( i, i+1 );
    }
  }
  
  // parent’s code
  
  // close all write fds 
  for ( i = 0; i < NUM_CHILDREN; i++ )
  {
    SYSCALL_ERROR( close( pipe_fds[i][1] ) );
  }
  
  // put all read fds in poll-array and set poll event type
  for ( i = 0; i < NUM_CHILDREN; i++) {
    poll_fd[i].fd = pipe_fds[i][0];
    poll_fd[i].events = POLLIN;
  }
  
  alive = NUM_CHILDREN;
  while( alive ) // keep polling and reading until all pipes are closed
  {
    // block until some read fds is ready for reading
    result = poll( poll_fd, NUM_CHILDREN, -1);
    SYSCALL_ERROR( result );
    
    // which read fds are ready for reading?
    for ( i = 0; i < NUM_CHILDREN; i++ )
    {	
      // important: first check POLLIN event before POLLHUP		
      if ( poll_fd[i].revents & POLLIN ) 
      {
	result = read( poll_fd[i].fd, recv_buf, MAX);
	SYSCALL_ERROR( result );
	printf("\nMessage received from child %d: %s", i, recv_buf);
	FFLUSH_ERROR( fflush( stdin ) ); 
      }
      if ( poll_fd[i].revents & POLLHUP ) 
      {
	alive--; // pipe closed by child
	poll_fd[i].fd = -1; // don't poll on this fd anymore
	poll_fd[i].events = 0;
      }
    }
  }
  
  // close all read fds 
  for ( i = 0; i < NUM_CHILDREN; i++ )
  {
    SYSCALL_ERROR( close( pipe_fds[i][0] ) );
  }
  
  // wait on termination of all child processes
  for ( i = 0; i < NUM_CHILDREN; i++ )
  {
    SYSCALL_ERROR( waitpid(child_pid[i], NULL, 0) );
  }	
  
  printf("\nParent process (pid = %d) is terminating ...\n", my_pid);
  
  exit(EXIT_SUCCESS);
}



